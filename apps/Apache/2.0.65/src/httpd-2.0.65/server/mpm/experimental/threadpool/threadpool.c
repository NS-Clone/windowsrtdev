/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* The purpose of this MPM is to fix the design flaws in the threaded
 * model.  Because of the way that pthreads and mutex locks interact,
 * it is basically impossible to cleanly gracefully shutdown a child
 * process if multiple threads are all blocked in accept.  This model
 * fixes those problems.
 */

#include "apr.h"
#include "apr_portable.h"
#include "apr_strings.h"
#include "apr_file_io.h"
#include "apr_thread_proc.h"
#include "apr_signal.h"
#include "apr_poll.h"
#include "apr_thread_mutex.h"
#include "apr_thread_cond.h"
#include "apr_proc_mutex.h"
#define APR_WANT_STRFUNC
#include "apr_want.h"

#if APR_HAVE_UNISTD_H
#include <unistd.h>
#endif
#if APR_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if APR_HAVE_SYS_WAIT_H
#include <sys/wait.h> 
#endif
#ifdef HAVE_SYS_PROCESSOR_H
#include <sys/processor.h> /* for bindprocessor() */
#endif

#if !APR_HAS_THREADS
#error The Worker MPM requires APR threads, but they are unavailable.
#endif

#define CORE_PRIVATE 
 
#include "ap_config.h"
#include "httpd.h" 
#include "http_main.h" 
#include "http_log.h" 
#include "http_config.h"        /* for read_config */ 
#include "http_core.h"          /* for get_remote_host */ 
#include "http_connection.h"
#include "ap_mpm.h"
#include "pod.h"
#include "mpm_common.h"
#include "ap_listen.h"
#include "scoreboard.h" 
#include "mpm_default.h"

#include <signal.h>
#include <limits.h>             /* for INT_MAX */

/* Limit on the total --- clients will be locked out if more servers than
 * this are needed.  It is intended solely to keep the server from crashing
 * when things get out of hand.
 *
 * We keep a hard maximum number of servers, for two reasons --- first off,
 * in case something goes seriously wrong, we want to stop the fork bomb
 * short of actually crashing the machine we're running on by filling some
 * kernel table.  Secondly, it keeps the size of the scoreboard file small
 * enough that we can read the whole thing without worrying too much about
 * the overhead.
 */
#ifndef DEFAULT_SERVER_LIMIT
#define DEFAULT_SERVER_LIMIT 16
#endif

/* Admin can't tune ServerLimit beyond MAX_SERVER_LIMIT.  We want
 * some sort of compile-time limit to help catch typos.
 */
#ifndef MAX_SERVER_LIMIT
#define MAX_SERVER_LIMIT 20000
#endif

/* Limit on the threads per process.  Clients will be locked out if more than
 * this  * server_limit are needed.
 *
 * We keep this for one reason it keeps the size of the scoreboard file small
 * enough that we can read the whole thing without worrying too much about
 * the overhead.
 */
#ifndef DEFAULT_THREAD_LIMIT
#define DEFAULT_THREAD_LIMIT 64 
#endif

/* Admin can't tune ThreadLimit beyond MAX_THREAD_LIMIT.  We want
 * some sort of compile-time limit to help catch typos.
 */
#ifndef MAX_THREAD_LIMIT
#define MAX_THREAD_LIMIT 20000
#endif

/*
 * Actual definitions of config globals
 */

int ap_threads_per_child = 0;         /* Worker threads per child */
static int ap_daemons_to_start = 0;
static int min_spare_threads = 0;
static int max_spare_threads = 0;
static int ap_daemons_limit = 0;
static int server_limit = DEFAULT_SERVER_LIMIT;
static int first_server_limit;
static int thread_limit = DEFAULT_THREAD_LIMIT;
static int first_thread_limit;
static int changed_limit_at_restart;
static int dying = 0;
static int workers_may_exit = 0;
static int start_thread_may_exit = 0;
static int listener_may_exit = 0;
static int requests_this_child;
static int num_listensocks = 0;
static int resource_shortage = 0;
static int mpm_state = AP_MPMQ_STARTING;

/* The structure used to pass unique initialization info to each thread */
typedef struct {
    int pid;
    int tid;
    int sd;
} proc_info;

/* Structure used to pass information to the thread responsible for 
 * creating the rest of the threads.
 */
typedef struct {
    apr_thread_t **threads;
    apr_thread_t *listener;
    int child_num_arg;
    apr_threadattr_t *threadattr;
} thread_starter;

#define ID_FROM_CHILD_THREAD(c, t)    ((c * thread_limit) + t)

/*
 * The max child slot ever assigned, preserved across restarts.  Necessary
 * to deal with MaxClients changes across AP_SIG_GRACEFUL restarts.  We 
 * use this value to optimize routines that have to scan the entire 
 * scoreboard.
 */
int ap_max_daemons_limit = -1;

static ap_pod_t *pod;

/* *Non*-shared http_main globals... */

server_rec *ap_server_conf;

/* The worker MPM respects a couple of runtime flags that can aid
 * in debugging. Setting the -DNO_DETACH flag will prevent the root process
 * from detaching from its controlling terminal. Additionally, setting
 * the -DONE_PROCESS flag (which implies -DNO_DETACH) will get you the
 * child_main loop running in the process which originally started up.
 * This gives you a pretty nice debugging environment.  (You'll get a SIGHUP
 * early in standalone_main; just continue through.  This is the server
 * trying to kill off any child processes which it might have lying
 * around --- Apache doesn't keep track of their pids, it just sends
 * SIGHUP to the process group, ignoring it in the root process.
 * Continue through and you'll be fine.).
 */

static int one_process = 0;

#ifdef DEBUG_SIGSTOP
int raise_sigstop_flags;
#endif

static apr_pool_t *pconf;                 /* Pool for config stuff */
static apr_pool_t *pchild;                /* Pool for httpd child stuff */

static pid_t ap_my_pid; /* Linux getpid() doesn't work except in main 
                           thread. Use this instead */
static pid_t parent_pid;
static apr_os_thread_t *listener_os_thread;

/* Locks for accept serialization */
static apr_proc_mutex_t *accept_mutex;

#if APR_O_NONBLOCK_INHERITED
#undef SINGLE_LISTEN_UNSERIALIZED_ACCEPT
#endif /* APR_O_NONBLOCK_INHERITED */

#ifdef SINGLE_LISTEN_UNSERIALIZED_ACCEPT
#define SAFE_ACCEPT(stmt) (ap_listeners->next ? (stmt) : APR_SUCCESS)
#else
#define SAFE_ACCEPT(stmt) (stmt)
#endif

/* The LISTENER_SIGNAL signal will be sent from the main thread to the 
 * listener thread to wake it up for graceful termination (what a child 
 * process from an old generation does when the admin does "apachectl 
 * graceful").  This signal will be blocked in all threads of a child
 * process except for the listener thread.
 */
#define LISTENER_SIGNAL     SIGHUP


/* Possible states of a worker thread. */
typedef enum {
    WORKER_IDLE,
    WORKER_BUSY,
    WORKER_TERMINATED
} worker_state_e;

/* Structure used to wake up an idle worker thread
 */
typedef struct {
    apr_pool_t *pool;
    apr_socket_t *csd;
    worker_state_e state;
    apr_thread_cond_t *cond;
    apr_thread_mutex_t *mutex;
} worker_wakeup_info;

/* Structure used to hold a stack of idle worker threads 
 */
typedef struct {
    apr_thread_mutex_t *mutex;
    apr_thread_cond_t *cond;
    worker_wakeup_info **stack;
    apr_size_t nelts;
    apr_size_t nalloc;
    int terminated;
} worker_stack;

static worker_stack* worker_stack_create(apr_pool_t *pool, apr_size_t max)
{
    apr_status_t rv;
    worker_stack *stack = (worker_stack *)apr_palloc(pool, sizeof(*stack));

    if ((rv = apr_thread_mutex_create(&stack->mutex, APR_THREAD_MUTEX_DEFAULT,
                                      pool)) != APR_SUCCESS) {
        return NULL;
    }
    if ((rv = apr_thread_cond_create(&stack->cond, pool)) != APR_SUCCESS) {
        return NULL;
    }
    stack->nelts = 0;
    stack->nalloc = max;
    stack->stack =
        (worker_wakeup_info **)apr_palloc(pool, stack->nalloc *
                                          sizeof(worker_wakeup_info *));
    stack->terminated = 0;
    return stack;
}

static apr_status_t worker_stack_wait(worker_stack *stack,
                                      worker_wakeup_info *wakeup)
{
    apr_status_t rv;

    wakeup->state = WORKER_IDLE;
    
    if ((rv = apr_thread_mutex_lock(stack->mutex)) != APR_SUCCESS) {
        return rv;
    }
    if (stack->terminated) {
        if ((rv = apr_thread_mutex_unlock(stack->mutex)) != APR_SUCCESS) {
            return rv;
        }
        return APR_EOF;
    }
    if (stack->nelts == stack->nalloc) {
        if ((rv = apr_thread_mutex_unlock(stack->mutex)) != APR_SUCCESS) {
            return rv;
        }
        return APR_ENOSPC;
    }
    stack->stack[stack->nelts] = wakeup;
    /* Signal a blocking listener thread only if we just made the
     * stack non-empty. */
    if (stack->nelts++ == 0) {
        (void)apr_thread_cond_signal(stack->cond);
    }
    if ((rv = apr_thread_mutex_unlock(stack->mutex)) != APR_SUCCESS) {
        return rv;
    }

    /* At this point we've already added this worker to the stack, now
     * we just wait until the listener has accept()ed a connection
     * for us. */
    if ((rv = apr_thread_mutex_lock(wakeup->mutex)) != APR_SUCCESS) {
        return rv;
    }
    while (wakeup->state == WORKER_IDLE) {
        if ((rv = apr_thread_cond_wait(wakeup->cond, wakeup->mutex)) !=
            APR_SUCCESS) {
            return rv;
        }
    }
    if ((rv = apr_thread_mutex_unlock(wakeup->mutex)) != APR_SUCCESS) {
        return rv;
    }
    return APR_SUCCESS;
}

static apr_status_t worker_stack_pop(worker_stack *stack,
                                     worker_wakeup_info **worker)
{
    apr_status_t rv;
    if ((rv = apr_thread_mutex_lock(stack->mutex)) != APR_SUCCESS) {
        return rv;
    }
    AP_DEBUG_ASSERT(stack->nelts >= 0);
    while ((stack->nelts == 0) && (!stack->terminated)) {
        rv = apr_thread_cond_wait(stack->cond, stack->mutex);
        if (rv != APR_SUCCESS) {
            apr_status_t rv2;
            rv2 = apr_thread_mutex_unlock(stack->mutex);
            if (rv2 != APR_SUCCESS) {
                return rv2;
            }
            return rv;
        }
    }
    if (stack->terminated) {
        if ((rv = apr_thread_mutex_unlock(stack->mutex)) != APR_SUCCESS) {
            return rv;
        }
        return APR_EOF;
    }
    *worker = stack->stack[--stack->nelts];
    if ((rv = apr_thread_mutex_unlock(stack->mutex)) != APR_SUCCESS) {
        return rv;
    }
    return APR_SUCCESS;
}

static apr_status_t worker_stack_terminate(worker_stack *stack)
{
    apr_status_t rv;
    worker_wakeup_info *worker;

    if ((rv = apr_thread_mutex_lock(stack->mutex)) != APR_SUCCESS) {
        return rv;
    }
    stack->terminated = 1;
    /* Wake up the listener thread. Although there will never be
     * more than one thread blocking on this condition, broadcast
     * just in case. */
    apr_thread_cond_broadcast(stack->cond);
    while (stack->nelts) {
        worker = stack->stack[--stack->nelts];
        apr_thread_mutex_lock(worker->mutex);
        worker->csd = 0;
        worker->state = WORKER_TERMINATED;
        apr_thread_cond_signal(worker->cond);
        apr_thread_mutex_unlock(worker->mutex);
    }
    if ((rv = apr_thread_mutex_unlock(stack->mutex)) != APR_SUCCESS) {
        return rv;
    }
    return APR_SUCCESS;
}

static worker_stack *idle_worker_stack;

static void wakeup_listener(void)
{
    apr_status_t rv;

    listener_may_exit = 1;
    if (!idle_worker_stack) {
        return;
    }
    if ((rv = apr_thread_mutex_lock(idle_worker_stack->mutex)) != APR_SUCCESS) {
        return;
    }
    if ((rv = apr_thread_cond_signal(idle_worker_stack->cond)) !=
        APR_SUCCESS) {
        return;
    }
    if ((rv = apr_thread_mutex_unlock(idle_worker_stack->mutex)) != APR_SUCCESS) {
        return;
    }
    if (!listener_os_thread) {
        /* XXX there is an obscure path that this doesn't handle perfectly:
         *     right after listener thread is created but before 
         *     listener_os_thread is set, the first worker thread hits an
         *     error and starts graceful termination
         */
        return;
    }
    /*
     * we should just be able to "kill(ap_my_pid, LISTENER_SIGNAL)" on all
     * platforms and wake up the listener thread since it is the only thread 
     * with SIGHUP unblocked, but that doesn't work on Linux
     */
#ifdef HAVE_PTHREAD_KILL
    pthread_kill(*listener_os_thread, LISTENER_SIGNAL);
#else
    kill(ap_my_pid, LISTENER_SIGNAL);
#endif
}

#define ST_INIT              0
#define ST_GRACEFUL          1
#define ST_UNGRACEFUL        2

static int terminate_mode = ST_INIT;

static void signal_threads(int mode)
{
    if (terminate_mode == mode) {
        return;
    }
    terminate_mode = mode;
    mpm_state = AP_MPMQ_STOPPING;

    /* in case we weren't called from the listener thread, wake up the
     * listener thread
     */
    wakeup_listener();

    /* for ungraceful termination, let the workers exit now;
     * for graceful termination, the listener thread will notify the
     * workers to exit once it has stopped accepting new connections
     */
    if (mode == ST_UNGRACEFUL) {
        workers_may_exit = 1;
        worker_stack_terminate(idle_worker_stack);
    }
}

AP_DECLARE(apr_status_t) ap_mpm_query(int query_code, int *result)
{
    switch(query_code){
        case AP_MPMQ_MAX_DAEMON_USED:
            *result = ap_max_daemons_limit;
            return APR_SUCCESS;
        case AP_MPMQ_IS_THREADED:
            *result = AP_MPMQ_STATIC;
            return APR_SUCCESS;
        case AP_MPMQ_IS_FORKED:
            *result = AP_MPMQ_DYNAMIC;
            return APR_SUCCESS;
        case AP_MPMQ_HARD_LIMIT_DAEMONS:
            *result = server_limit;
            return APR_SUCCESS;
        case AP_MPMQ_HARD_LIMIT_THREADS:
            *result = thread_limit;
            return APR_SUCCESS;
        case AP_MPMQ_MAX_THREADS:
            *result = ap_threads_per_child;
            return APR_SUCCESS;
        case AP_MPMQ_MIN_SPARE_DAEMONS:
            *result = 0;
            return APR_SUCCESS;
        case AP_MPMQ_MIN_SPARE_THREADS:    
            *result = min_spare_threads;
            return APR_SUCCESS;
        case AP_MPMQ_MAX_SPARE_DAEMONS:
            *result = 0;
            return APR_SUCCESS;
        case AP_MPMQ_MAX_SPARE_THREADS:
            *result = max_spare_threads;
            return APR_SUCCESS;
        case AP_MPMQ_MAX_REQUESTS_DAEMON:
            *result = ap_max_requests_per_child;
            return APR_SUCCESS;
        case AP_MPMQ_MAX_DAEMONS:
            *result = ap_daemons_limit;
            return APR_SUCCESS;
        case AP_MPMQ_MPM_STATE:
            *result = mpm_state;
            return APR_SUCCESS;
    }
    return APR_ENOTIMPL;
}

/* a clean exit from a child with proper cleanup */ 
static void clean_child_exit(int code) __attribute__ ((noreturn));
static void clean_child_exit(int code)
{
    mpm_state = AP_MPMQ_STOPPING;
    if (pchild) {
        apr_pool_destroy(pchild);
    }
    exit(code);
}

static void just_die(int sig)
{
    clean_child_exit(0);
}

/*****************************************************************
 * Connection structures and accounting...
 */

/* volatile just in case */
static int volatile shutdown_pending;
static int volatile restart_pending;
static int volatile is_graceful;
static volatile int child_fatal;
ap_generation_t volatile ap_my_generation;

/*
 * ap_start_shutdown() and ap_start_restart(), below, are a first stab at
 * functions to initiate shutdown or restart without relying on signals. 
 * Previously this was initiated in sig_term() and restart() signal handlers, 
 * but we want to be able to start a shutdown/restart from other sources --
 * e.g. on Win32, from the service manager. Now the service manager can
 * call ap_start_shutdown() or ap_start_restart() as appropiate.  Note that
 * these functions can also be called by the child processes, since global
 * variables are no longer used to pass on the required action to the parent.
 *
 * These should only be called from the parent process itself, since the
 * parent process will use the shutdown_pending and restart_pending variables
 * to determine whether to shutdown or restart. The child process should
 * call signal_parent() directly to tell the parent to die -- this will
 * cause neither of those variable to be set, which the parent will
 * assume means something serious is wrong (which it will be, for the
 * child to force an exit) and so do an exit anyway.
 */

static void ap_start_shutdown(void)
{
    mpm_state = AP_MPMQ_STOPPING;
    if (shutdown_pending == 1) {
        /* Um, is this _probably_ not an error, if the user has
         * tried to do a shutdown twice quickly, so we won't
         * worry about reporting it.
         */
        return;
    }
    shutdown_pending = 1;
}

/* do a graceful restart if graceful == 1 */
static void ap_start_restart(int graceful)
{
    mpm_state = AP_MPMQ_STOPPING;
    if (restart_pending == 1) {
        /* Probably not an error - don't bother reporting it */
        return;
    }
    restart_pending = 1;
    is_graceful = graceful;
}

static void sig_term(int sig)
{
    ap_start_shutdown();
}

static void restart(int sig)
{
    ap_start_restart(sig == AP_SIG_GRACEFUL);
}

static void set_signals(void)
{
#ifndef NO_USE_SIGACTION
    struct sigaction sa;
#endif

    if (!one_process) {
        ap_fatal_signal_setup(ap_server_conf, pconf);
    }

#ifndef NO_USE_SIGACTION
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = sig_term;
    if (sigaction(SIGTERM, &sa, NULL) < 0)
        ap_log_error(APLOG_MARK, APLOG_WARNING, errno, ap_server_conf, 
                     "sigaction(SIGTERM)");
#ifdef SIGINT
    if (sigaction(SIGINT, &sa, NULL) < 0)
        ap_log_error(APLOG_MARK, APLOG_WARNING, errno, ap_server_conf, 
                     "sigaction(SIGINT)");
#endif
#ifdef SIGXCPU
    sa.sa_handler = SIG_DFL;
    if (sigaction(SIGXCPU, &sa, NULL) < 0)
        ap_log_error(APLOG_MARK, APLOG_WARNING, errno, ap_server_conf, 
                     "sigaction(SIGXCPU)");
#endif
#ifdef SIGXFSZ
    sa.sa_handler = SIG_DFL;
    if (sigaction(SIGXFSZ, &sa, NULL) < 0)
        ap_log_error(APLOG_MARK, APLOG_WARNING, errno, ap_server_conf, 
                     "sigaction(SIGXFSZ)");
#endif
#ifdef SIGPIPE
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) < 0)
        ap_log_error(APLOG_MARK, APLOG_WARNING, errno, ap_server_conf, 
                     "sigaction(SIGPIPE)");
#endif

    /* we want to ignore HUPs and AP_SIG_GRACEFUL while we're busy 
     * processing one */
    sigaddset(&sa.sa_mask, SIGHUP);
    sigaddset(&sa.sa_mask, AP_SIG_GRACEFUL);
    sa.sa_handler = restart;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        ap_log_error(APLOG_MARK, APLOG_WARNING, errno, ap_server_conf, 
                     "sigaction(SIGHUP)");
    if (sigaction(AP_SIG_GRACEFUL, &sa, NULL) < 0)
        ap_log_error(APLOG_MARK, APLOG_WARNING, errno, ap_server_conf, 
                     "sigaction(" AP_SIG_GRACEFUL_STRING ")");
#else
    if (!one_process) {
#ifdef SIGXCPU
        apr_signal(SIGXCPU, SIG_DFL);
#endif /* SIGXCPU */
#ifdef SIGXFSZ
        apr_signal(SIGXFSZ, SIG_DFL);
#endif /* SIGXFSZ */
    }

    apr_signal(SIGTERM, sig_term);
#ifdef SIGHUP
    apr_signal(SIGHUP, restart);
#endif /* SIGHUP */
#ifdef AP_SIG_GRACEFUL
    apr_signal(AP_SIG_GRACEFUL, restart);
#endif /* AP_SIG_GRACEFUL */
#ifdef SIGPIPE
    apr_signal(SIGPIPE, SIG_IGN);
#endif /* SIGPIPE */

#endif
}

/*****************************************************************
 * Here follows a long bunch of generic server bookkeeping stuff...
 */

int ap_graceful_stop_signalled(void)
    /* XXX this is really a bad confusing obsolete name
     * maybe it should be ap_mpm_process_exiting?
     */
{
    /* note: for a graceful termination, listener_may_exit will be set before
     *       workers_may_exit, so check listener_may_exit
     */
    return listener_may_exit;
}

/*****************************************************************
 * Child process main loop.
 */

static void process_socket(apr_pool_t *p, apr_socket_t *sock, int my_child_num,
                           int my_thread_num, apr_bucket_alloc_t *bucket_alloc)
{
    conn_rec *current_conn;
    long conn_id = ID_FROM_CHILD_THREAD(my_child_num, my_thread_num);
    int csd;
    ap_sb_handle_t *sbh;

    ap_create_sb_handle(&sbh, p, my_child_num, my_thread_num);
    apr_os_sock_get(&csd, sock);

    current_conn = ap_run_create_connection(p, ap_server_conf, sock,
                                            conn_id, sbh, bucket_alloc);
    if (current_conn) {
        ap_process_connection(current_conn, sock);
        ap_lingering_close(current_conn);
    }
}

/* requests_this_child has gone to zero or below.  See if the admin coded
   "MaxRequestsPerChild 0", and keep going in that case.  Doing it this way
   simplifies the hot path in worker_thread */
static void check_infinite_requests(void)
{
    if (ap_max_requests_per_child) {
        signal_threads(ST_GRACEFUL);
    }
    else {
        /* wow! if you're executing this code, you may have set a record.
         * either this child process has served over 2 billion requests, or
         * you're running a threaded 2.0 on a 16 bit machine.  
         *
         * I'll buy pizza and beers at Apachecon for the first person to do
         * the former without cheating (dorking with INT_MAX, or running with
         * uncommitted performance patches, for example).    
         *
         * for the latter case, you probably deserve a beer too.   Greg Ames
         */
            
        requests_this_child = INT_MAX;      /* keep going */ 
    }
}

static void unblock_signal(int sig)
{
    sigset_t sig_mask;

    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, sig);
#if defined(SIGPROCMASK_SETS_THREAD_MASK)
    sigprocmask(SIG_UNBLOCK, &sig_mask, NULL);
#else
    pthread_sigmask(SIG_UNBLOCK, &sig_mask, NULL);
#endif
}

static void dummy_signal_handler(int sig)
{
    /* XXX If specifying SIG_IGN is guaranteed to unblock a syscall,
     *     then we don't need this goofy function.
     */
}

static void *listener_thread(apr_thread_t *thd, void * dummy)
{
    proc_info * ti = dummy;
    int process_slot = ti->pid;
    apr_pool_t *tpool = apr_thread_pool_get(thd);
    void *csd = NULL;
    apr_pool_t *ptrans;                /* Pool for per-transaction stuff */
    int n;
    apr_pollfd_t *pollset;
    apr_status_t rv;
    ap_listen_rec *lr, *last_lr = ap_listeners;
    worker_wakeup_info *worker = NULL;

    free(ti);

    apr_poll_setup(&pollset, num_listensocks, tpool);
    for(lr = ap_listeners ; lr != NULL ; lr = lr->next)
        apr_poll_socket_add(pollset, lr->sd, APR_POLLIN);

    /* Unblock the signal used to wake this thread up, and set a handler for
     * it.
     */
    unblock_signal(LISTENER_SIGNAL);
    apr_signal(LISTENER_SIGNAL, dummy_signal_handler);

    /* TODO: Switch to a system where threads reuse the results from earlier
       poll calls - manoj */
    while (1) {
        /* TODO: requests_this_child should be synchronized - aaron */
        if (requests_this_child <= 0) {
            check_infinite_requests();
        }
        if (listener_may_exit) break;

        if (worker == NULL) {
            rv = worker_stack_pop(idle_worker_stack, &worker);
            if (APR_STATUS_IS_EOF(rv)) {
                break;
            }
            else if (rv != APR_SUCCESS) {
                ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf,
                             "worker_stack_pop failed");
                break;
            }
            ptrans = worker->pool;
        }
        AP_DEBUG_ASSERT(worker->state == WORKER_IDLE);

        if ((rv = SAFE_ACCEPT(apr_proc_mutex_lock(accept_mutex)))
            != APR_SUCCESS) {
            int level = APLOG_EMERG;

            if (listener_may_exit) {
                break;
            }
            if (ap_scoreboard_image->parent[process_slot].generation != 
                ap_scoreboard_image->global->running_generation) {
                level = APLOG_DEBUG; /* common to get these at restart time */
            }
            ap_log_error(APLOG_MARK, level, rv, ap_server_conf,
                         "apr_proc_mutex_lock failed. Attempting to shutdown "
                         "process gracefully.");
            signal_threads(ST_GRACEFUL);
            break;                    /* skip the lock release */
        }

        if (!APR_O_NONBLOCK_INHERITED && !ap_listeners->next) {
            /* Only one listener, so skip the poll */
            lr = ap_listeners;
        }
        else {
            while (!listener_may_exit) {
                apr_status_t ret;
                apr_int16_t event;

                ret = apr_poll(pollset, num_listensocks, &n, -1);
                if (ret != APR_SUCCESS) {
                    if (APR_STATUS_IS_EINTR(ret)) {
                        continue;
                    }

                    /* apr_poll() will only return errors in catastrophic
                     * circumstances. Let's try exiting gracefully, for now. */
                    ap_log_error(APLOG_MARK, APLOG_ERR, ret, (const server_rec *)
                                 ap_server_conf, "apr_poll: (listen)");
                    signal_threads(ST_GRACEFUL);
                }

                if (listener_may_exit) break;

                /* find a listener */
                lr = last_lr;
                do {
                    lr = lr->next;
                    if (lr == NULL) {
                        lr = ap_listeners;
                    }
                    /* XXX: Should we check for POLLERR? */
                    apr_poll_revents_get(&event, lr->sd, pollset);
                    if (event & APR_POLLIN) {
                        last_lr = lr;
                        goto got_fd;
                    }
                } while (lr != last_lr);
            }
        }
    got_fd:
        if (!listener_may_exit) {
            rv = lr->accept_func(&csd, lr, ptrans);
            /* later we trash rv and rely on csd to indicate success/failure */
            AP_DEBUG_ASSERT(rv == APR_SUCCESS || !csd);

            if (rv == APR_EGENERAL) {
                /* E[NM]FILE, ENOMEM, etc */
                resource_shortage = 1;
                signal_threads(ST_GRACEFUL);
            }
            if ((rv = SAFE_ACCEPT(apr_proc_mutex_unlock(accept_mutex)))
                != APR_SUCCESS) {
                int level = APLOG_EMERG;

                if (listener_may_exit) {
                    break;
                }
                if (ap_scoreboard_image->parent[process_slot].generation != 
                    ap_scoreboard_image->global->running_generation) {
                    level = APLOG_DEBUG; /* common to get these at restart time */
                }
                ap_log_error(APLOG_MARK, level, rv, ap_server_conf,
                             "apr_proc_mutex_unlock failed. Attempting to "
                             "shutdown process gracefully.");
                signal_threads(ST_GRACEFUL);
            }
            if (csd != NULL) {
                /* Wake up the sleeping worker. */
                apr_thread_mutex_lock(worker->mutex);
                worker->csd = (apr_socket_t *)csd;
                worker->state = WORKER_BUSY;
                /* Posix allows us to signal this condition without
                 * owning the associated mutex, but in that case it can
                 * not guarantee predictable scheduling. See
                 * _UNIX Network Programming: Interprocess Communication_
                 * by W. Richard Stevens, Vol 2, 2nd Ed, pp. 170-171. */
                apr_thread_cond_signal(worker->cond);
                apr_thread_mutex_unlock(worker->mutex);
                worker = NULL;
            }
        }
        else {
            if ((rv = SAFE_ACCEPT(apr_proc_mutex_unlock(accept_mutex)))
                != APR_SUCCESS) {
                ap_log_error(APLOG_MARK, APLOG_EMERG, rv, ap_server_conf,
                             "apr_proc_mutex_unlock failed. Attempting to "
                             "shutdown process gracefully.");
                signal_threads(ST_GRACEFUL);
            }
            break;
        }
    }

    workers_may_exit = 1;
    if (worker) {
        apr_thread_mutex_lock(worker->mutex);
        worker->state = WORKER_TERMINATED;
        /* Posix allows us to signal this condition without
         * owning the associated mutex, but in that case it can
         * not guarantee predictable scheduling. See
         * _UNIX Network Programming: Interprocess Communication_
         * by W. Richard Stevens, Vol 2, 2nd Ed, pp. 170-171. */
        apr_thread_cond_signal(worker->cond);
        apr_thread_mutex_unlock(worker->mutex);
    }
    worker_stack_terminate(idle_worker_stack);
    dying = 1;
    ap_scoreboard_image->parent[process_slot].quiescing = 1;

    /* wake up the main thread */
    kill(ap_my_pid, SIGTERM);

    apr_thread_exit(thd, APR_SUCCESS);
    return NULL;
}

/* XXX For ungraceful termination/restart, we definitely don't want to
 *     wait for active connections to finish but we may want to wait
 *     for idle workers to get out of the queue code and release mutexes,
 *     since those mutexes are cleaned up pretty soon and some systems
 *     may not react favorably (i.e., segfault) if operations are attempted
 *     on cleaned-up mutexes.
 */
static void * APR_THREAD_FUNC worker_thread(apr_thread_t *thd, void * dummy)
{
    proc_info * ti = dummy;
    int process_slot = ti->pid;
    int thread_slot = ti->tid;
    apr_bucket_alloc_t *bucket_alloc;
    apr_pool_t *tpool = apr_thread_pool_get(thd);
    apr_pool_t *ptrans;                /* Pool for per-transaction stuff */
    apr_allocator_t *allocator;
    apr_status_t rv;
    worker_wakeup_info *wakeup;

    free(ti);

    ap_update_child_status_from_indexes(process_slot, thread_slot, SERVER_STARTING, NULL);

    apr_allocator_create(&allocator);
    apr_allocator_max_free_set(allocator, ap_max_mem_free);
    /* XXX: why is ptrans's parent not tpool? --jcw 08/2003 */
    apr_pool_create_ex(&ptrans, NULL, NULL, allocator);
    apr_allocator_owner_set(allocator, ptrans);
    bucket_alloc = apr_bucket_alloc_create_ex(allocator);

    wakeup = (worker_wakeup_info *)apr_palloc(tpool, sizeof(*wakeup));
    wakeup->pool = ptrans;
    if ((rv = apr_thread_cond_create(&wakeup->cond, tpool)) != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_EMERG, rv, ap_server_conf,
                     "apr_thread_cond_create failed. Attempting to shutdown "
                     "process gracefully.");
        signal_threads(ST_GRACEFUL);
        apr_thread_exit(thd, rv);
    }
    if ((rv = apr_thread_mutex_create(&wakeup->mutex, APR_THREAD_MUTEX_DEFAULT,
                                      tpool)) != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_EMERG, rv, ap_server_conf,
                     "apr_thread_mutex_create failed. Attempting to shutdown "
                     "process gracefully.");
        signal_threads(ST_GRACEFUL);
        apr_thread_exit(thd, rv);
    }

    while (!workers_may_exit) {
        ap_update_child_status_from_indexes(process_slot, thread_slot, SERVER_READY, NULL);
        rv = worker_stack_wait(idle_worker_stack, wakeup);
        if (APR_STATUS_IS_EOF(rv)) {
            break; /* The queue has been terminated. */
        }
        else if (rv != APR_SUCCESS) {
            ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf,
                         "worker_stack_wait failed");
            break; /* Treat all other errors as fatal. */
        }
        else if (wakeup->state == WORKER_TERMINATED) {
            break; /* They told us to quit. */
        }
        AP_DEBUG_ASSERT(wakeup->state != WORKER_IDLE);
        process_socket(ptrans, wakeup->csd,
                       process_slot, thread_slot, bucket_alloc);
        requests_this_child--; /* FIXME: should be synchronized - aaron */
        apr_pool_clear(ptrans);
    }

    ap_update_child_status_from_indexes(process_slot, thread_slot,
        (dying) ? SERVER_DEAD : SERVER_GRACEFUL, (request_rec *) NULL);

    apr_bucket_alloc_destroy(bucket_alloc);

    apr_thread_exit(thd, APR_SUCCESS);
    return NULL;
}

static int check_signal(int signum)
{
    switch (signum) {
    case SIGTERM:
    case SIGINT:
        return 1;
    }
    return 0;
}

static void create_listener_thread(thread_starter *ts)
{
    int my_child_num = ts->child_num_arg;
    apr_threadattr_t *thread_attr = ts->threadattr;
    proc_info *my_info;
    apr_status_t rv;

    my_info = (proc_info *)malloc(sizeof(proc_info));
    my_info->pid = my_child_num;
    my_info->tid = -1; /* listener thread doesn't have a thread slot */
    my_info->sd = 0;
    rv = apr_thread_create(&ts->listener, thread_attr, listener_thread,
                           my_info, pchild);
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_ALERT, rv, ap_server_conf,
                     "apr_thread_create: unable to create listener thread");
        /* In case system resources are maxxed out, we don't want
         * Apache running away with the CPU trying to fork over and
         * over and over again if we exit.
         * XXX Jeff doesn't see how Apache is going to try to fork again since
         * the exit code is APEXIT_CHILDFATAL
         */
        apr_sleep(10 * APR_USEC_PER_SEC);
        clean_child_exit(APEXIT_CHILDFATAL);
    }
    apr_os_thread_get(&listener_os_thread, ts->listener);
}

/* XXX under some circumstances not understood, children can get stuck
 *     in start_threads forever trying to take over slots which will
 *     never be cleaned up; for now there is an APLOG_DEBUG message issued
 *     every so often when this condition occurs
 */
static void * APR_THREAD_FUNC start_threads(apr_thread_t *thd, void *dummy)
{
    thread_starter *ts = dummy;
    apr_thread_t **threads = ts->threads;
    apr_threadattr_t *thread_attr = ts->threadattr;
    int child_num_arg = ts->child_num_arg;
    int my_child_num = child_num_arg;
    proc_info *my_info;
    apr_status_t rv;
    int i;
    int threads_created = 0;
    int loops;
    int prev_threads_created;

    idle_worker_stack = worker_stack_create(pchild, ap_threads_per_child);
    if (idle_worker_stack == NULL) {
        ap_log_error(APLOG_MARK, APLOG_ALERT, 0, ap_server_conf,
                     "worker_stack_create() failed");
        clean_child_exit(APEXIT_CHILDFATAL);
    }

    loops = prev_threads_created = 0;
    while (1) {
        /* ap_threads_per_child does not include the listener thread */
        for (i = 0; i < ap_threads_per_child; i++) {
            int status = ap_scoreboard_image->servers[child_num_arg][i].status;

            if (status != SERVER_GRACEFUL && status != SERVER_DEAD) {
                continue;
            }

            my_info = (proc_info *)malloc(sizeof(proc_info));
            if (my_info == NULL) {
                ap_log_error(APLOG_MARK, APLOG_ALERT, errno, ap_server_conf,
                             "malloc: out of memory");
                clean_child_exit(APEXIT_CHILDFATAL);
            }
            my_info->pid = my_child_num;
            my_info->tid = i;
            my_info->sd = 0;
        
            /* We are creating threads right now */
            ap_update_child_status_from_indexes(my_child_num, i,
                                                SERVER_STARTING, NULL);
            /* We let each thread update its own scoreboard entry.  This is
             * done because it lets us deal with tid better.
             */
            rv = apr_thread_create(&threads[i], thread_attr, 
                                   worker_thread, my_info, pchild);
            if (rv != APR_SUCCESS) {
                ap_log_error(APLOG_MARK, APLOG_ALERT, rv, ap_server_conf,
                    "apr_thread_create: unable to create worker thread");
                /* In case system resources are maxxed out, we don't want
                   Apache running away with the CPU trying to fork over and
                   over and over again if we exit. */
                apr_sleep(10 * APR_USEC_PER_SEC);
                clean_child_exit(APEXIT_CHILDFATAL);
            }
            threads_created++;
            if (threads_created == 1) {
                /* now that we have a worker thread, it makes sense to create
                 * a listener thread (we don't want a listener without a worker!)
                 */
                create_listener_thread(ts);
            }
        }
        if (start_thread_may_exit || threads_created == ap_threads_per_child) {
            break;
        }
        /* wait for previous generation to clean up an entry */
        apr_sleep(1 * APR_USEC_PER_SEC);
        ++loops;
        if (loops % 120 == 0) { /* every couple of minutes */
            if (prev_threads_created == threads_created) {
                ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, ap_server_conf,
                             "child %" APR_PID_T_FMT " isn't taking over "
                             "slots very quickly (%d of %d)",
                             ap_my_pid, threads_created, ap_threads_per_child);
            }
            prev_threads_created = threads_created;
        }
    }
    
    /* What state should this child_main process be listed as in the 
     * scoreboard...?
     *  ap_update_child_status_from_indexes(my_child_num, i, SERVER_STARTING, 
     *                                      (request_rec *) NULL);
     * 
     *  This state should be listed separately in the scoreboard, in some kind
     *  of process_status, not mixed in with the worker threads' status.   
     *  "life_status" is almost right, but it's in the worker's structure, and 
     *  the name could be clearer.   gla
     */
    apr_thread_exit(thd, APR_SUCCESS);
    return NULL;
}

static void join_workers(apr_thread_t *listener, apr_thread_t **threads)
{
    int i;
    apr_status_t rv, thread_rv;

    if (listener) {
        int iter;

        /* deal with a rare timing window which affects waking up the
         * listener thread...  if the signal sent to the listener thread
         * is delivered between the time it verifies that the
         * listener_may_exit flag is clear and the time it enters a
         * blocking syscall, the signal didn't do any good...  work around
         * that by sleeping briefly and sending it again
         */

        iter = 0;
        while (iter < 10 && 
#ifdef HAVE_PTHREAD_KILL
               pthread_kill(*listener_os_thread, 0)
#else
               kill(ap_my_pid, 0)
#endif
               == 0) {
            /* listener not dead yet */
            apr_sleep(APR_USEC_PER_SEC / 2);
            wakeup_listener();
            ++iter;
        }
        if (iter >= 10) {
            ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, ap_server_conf,
                         "the listener thread didn't exit");
        }
        else {
            rv = apr_thread_join(&thread_rv, listener);
            if (rv != APR_SUCCESS) {
                ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf,
                             "apr_thread_join: unable to join listener thread");
            }
        }
    }
    
    for (i = 0; i < ap_threads_per_child; i++) {
        if (threads[i]) { /* if we ever created this thread */
            rv = apr_thread_join(&thread_rv, threads[i]);
            if (rv != APR_SUCCESS) {
                ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf,
                             "apr_thread_join: unable to join worker "
                             "thread %d",
                             i);
            }
        }
    }
}

static void join_start_thread(apr_thread_t *start_thread_id)
{
    apr_status_t rv, thread_rv;

    start_thread_may_exit = 1; /* tell it to give up in case it is still 
                                * trying to take over slots from a 
                                * previous generation
                                */
    rv = apr_thread_join(&thread_rv, start_thread_id);
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf,
                     "apr_thread_join: unable to join the start "
                     "thread");
    }
}

static void child_main(int child_num_arg)
{
    apr_thread_t **threads;
    apr_status_t rv;
    thread_starter *ts;
    apr_threadattr_t *thread_attr;
    apr_thread_t *start_thread_id;

    mpm_state = AP_MPMQ_STARTING; /* for benefit of any hooks that run as this
                                   * child initializes
                                   */
    ap_my_pid = getpid();
    ap_fatal_signal_child_setup(ap_server_conf);
    apr_pool_create(&pchild, pconf);

    /*stuff to do before we switch id's, so we have permissions.*/
    ap_reopen_scoreboard(pchild, NULL, 0);

    rv = SAFE_ACCEPT(apr_proc_mutex_child_init(&accept_mutex, ap_lock_fname,
                                               pchild));
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_EMERG, rv, ap_server_conf,
                     "Couldn't initialize cross-process lock in child");
        clean_child_exit(APEXIT_CHILDFATAL);
    }

    if (unixd_setup_child()) {
        clean_child_exit(APEXIT_CHILDFATAL);
    }

    ap_run_child_init(pchild, ap_server_conf);

    /* done with init critical section */

    /* Just use the standard apr_setup_signal_thread to block all signals
     * from being received.  The child processes no longer use signals for
     * any communication with the parent process.
     */
    rv = apr_setup_signal_thread();
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_EMERG, rv, ap_server_conf,
                     "Couldn't initialize signal thread");
        clean_child_exit(APEXIT_CHILDFATAL);
    }

    if (ap_max_requests_per_child) {
        requests_this_child = ap_max_requests_per_child;
    }
    else {
        /* coding a value of zero means infinity */
        requests_this_child = INT_MAX;
    }
    
    /* Setup worker threads */

    /* clear the storage; we may not create all our threads immediately, 
     * and we want a 0 entry to indicate a thread which was not created
     */
    threads = (apr_thread_t **)calloc(1, 
                                sizeof(apr_thread_t *) * ap_threads_per_child);
    if (threads == NULL) {
        ap_log_error(APLOG_MARK, APLOG_ALERT, errno, ap_server_conf,
                     "malloc: out of memory");
        clean_child_exit(APEXIT_CHILDFATAL);
    }

    ts = (thread_starter *)apr_palloc(pchild, sizeof(*ts));

    apr_threadattr_create(&thread_attr, pchild);
    /* 0 means PTHREAD_CREATE_JOINABLE */
    apr_threadattr_detach_set(thread_attr, 0);

    ts->threads = threads;
    ts->listener = NULL;
    ts->child_num_arg = child_num_arg;
    ts->threadattr = thread_attr;

    rv = apr_thread_create(&start_thread_id, thread_attr, start_threads,
                           ts, pchild);
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_ALERT, rv, ap_server_conf,
                     "apr_thread_create: unable to create worker thread");
        /* In case system resources are maxxed out, we don't want
           Apache running away with the CPU trying to fork over and
           over and over again if we exit. */
        apr_sleep(10 * APR_USEC_PER_SEC);
        clean_child_exit(APEXIT_CHILDFATAL);
    }

    mpm_state = AP_MPMQ_RUNNING;
    
    /* If we are only running in one_process mode, we will want to
     * still handle signals. */
    if (one_process) {
        /* Block until we get a terminating signal. */
        apr_signal_thread(check_signal);
        /* make sure the start thread has finished; signal_threads() 
         * and join_workers() depend on that
         */
        /* XXX join_start_thread() won't be awakened if one of our
         *     threads encounters a critical error and attempts to
         *     shutdown this child
         */
        join_start_thread(start_thread_id);
        signal_threads(ST_UNGRACEFUL); /* helps us terminate a little more
                           * quickly than the dispatch of the signal thread
                           * beats the Pipe of Death and the browsers
                           */
        /* A terminating signal was received. Now join each of the
         * workers to clean them up.
         *   If the worker already exited, then the join frees
         *   their resources and returns.
         *   If the worker hasn't exited, then this blocks until
         *   they have (then cleans up).
         */
        join_workers(ts->listener, threads);
    }
    else { /* !one_process */
        /* remove SIGTERM from the set of blocked signals...  if one of
         * the other threads in the process needs to take us down
         * (e.g., for MaxRequestsPerChild) it will send us SIGTERM
         */
        unblock_signal(SIGTERM);
        apr_signal(SIGTERM, dummy_signal_handler);
        /* Watch for any messages from the parent over the POD */
        while (1) {
            rv = ap_mpm_pod_check(pod);
            if (rv == AP_NORESTART) {
                /* see if termination was triggered while we slept */
                switch(terminate_mode) {
                case ST_GRACEFUL:
                    rv = AP_GRACEFUL;
                    break;
                case ST_UNGRACEFUL:
                    rv = AP_RESTART;
                    break;
                }
            }
            if (rv == AP_GRACEFUL || rv == AP_RESTART) {
                /* make sure the start thread has finished; 
                 * signal_threads() and join_workers depend on that
                 */
                join_start_thread(start_thread_id);
                signal_threads(rv == AP_GRACEFUL ? ST_GRACEFUL : ST_UNGRACEFUL);
                break;
            }
        }

        if (rv == AP_GRACEFUL) {
            /* A terminating signal was received. Now join each of the
             * workers to clean them up.
             *   If the worker already exited, then the join frees
             *   their resources and returns.
             *   If the worker hasn't exited, then this blocks until
             *   they have (then cleans up).
             */
            join_workers(ts->listener, threads);
        }
    }

    free(threads);

    clean_child_exit(resource_shortage ? APEXIT_CHILDSICK : 0);
}

static int make_child(server_rec *s, int slot) 
{
    int pid;

    if (slot + 1 > ap_max_daemons_limit) {
        ap_max_daemons_limit = slot + 1;
    }

    if (one_process) {
        set_signals();
        ap_scoreboard_image->parent[slot].pid = getpid();
        child_main(slot);
    }

    if ((pid = fork()) == -1) {
        ap_log_error(APLOG_MARK, APLOG_ERR, errno, s, 
                     "fork: Unable to fork new process");

        /* fork didn't succeed. Fix the scoreboard or else
         * it will say SERVER_STARTING forever and ever
         */
        ap_update_child_status_from_indexes(slot, 0, SERVER_DEAD, NULL);

        /* In case system resources are maxxed out, we don't want
           Apache running away with the CPU trying to fork over and
           over and over again. */
        apr_sleep(10 * APR_USEC_PER_SEC);

        return -1;
    }

    if (!pid) {
#ifdef HAVE_BINDPROCESSOR
        /* By default, AIX binds to a single processor.  This bit unbinds
         * children which will then bind to another CPU.
         */
        int status = bindprocessor(BINDPROCESS, (int)getpid(),
                               PROCESSOR_CLASS_ANY);
        if (status != OK)
            ap_log_error(APLOG_MARK, APLOG_WARNING, errno, 
                         ap_server_conf,
                         "processor unbind failed %d", status);
#endif
        RAISE_SIGSTOP(MAKE_CHILD);

        apr_signal(SIGTERM, just_die);
        child_main(slot);

        clean_child_exit(0);
    }
    /* else */
    ap_scoreboard_image->parent[slot].quiescing = 0;
    ap_scoreboard_image->parent[slot].pid = pid;
    return 0;
}

/* start up a bunch of children */
static void startup_children(int number_to_start)
{
    int i;

    for (i = 0; number_to_start && i < ap_daemons_limit; ++i) {
        if (ap_scoreboard_image->parent[i].pid != 0) {
            continue;
        }
        if (make_child(ap_server_conf, i) < 0) {
            break;
        }
        --number_to_start;
    }
}


/*
 * idle_spawn_rate is the number of children that will be spawned on the
 * next maintenance cycle if there aren't enough idle servers.  It is
 * doubled up to MAX_SPAWN_RATE, and reset only when a cycle goes by
 * without the need to spawn.
 */
static int idle_spawn_rate = 1;
#ifndef MAX_SPAWN_RATE
#define MAX_SPAWN_RATE        (32)
#endif
static int hold_off_on_exponential_spawning;

static void perform_idle_server_maintenance(void)
{
    int i, j;
    int idle_thread_count;
    worker_score *ws;
    process_score *ps;
    int free_length;
    int totally_free_length = 0;
    int free_slots[MAX_SPAWN_RATE];
    int last_non_dead;
    int total_non_dead;

    /* initialize the free_list */
    free_length = 0;

    idle_thread_count = 0;
    last_non_dead = -1;
    total_non_dead = 0;

    for (i = 0; i < ap_daemons_limit; ++i) {
        /* Initialization to satisfy the compiler. It doesn't know
         * that ap_threads_per_child is always > 0 */
        int status = SERVER_DEAD;
        int any_dying_threads = 0;
        int any_dead_threads = 0;
        int all_dead_threads = 1;

        if (i >= ap_max_daemons_limit && totally_free_length == idle_spawn_rate)
            break;
        ps = &ap_scoreboard_image->parent[i];
        for (j = 0; j < ap_threads_per_child; j++) {
            ws = &ap_scoreboard_image->servers[i][j];
            status = ws->status;

            /* XXX any_dying_threads is probably no longer needed    GLA */
            any_dying_threads = any_dying_threads || 
                                (status == SERVER_GRACEFUL);
            any_dead_threads = any_dead_threads || (status == SERVER_DEAD);
            all_dead_threads = all_dead_threads &&
                                   (status == SERVER_DEAD ||
                                    status == SERVER_GRACEFUL);

            /* We consider a starting server as idle because we started it
             * at least a cycle ago, and if it still hasn't finished starting
             * then we're just going to swamp things worse by forking more.
             * So we hopefully won't need to fork more if we count it.
             * This depends on the ordering of SERVER_READY and SERVER_STARTING.
             */
            if (status <= SERVER_READY && status != SERVER_DEAD &&
                    !ps->quiescing &&
                    ps->generation == ap_my_generation &&
                 /* XXX the following shouldn't be necessary if we clean up 
                  *     properly after seg faults, but we're not yet    GLA 
                  */     
                    ps->pid != 0) {
                ++idle_thread_count;
            }
        }
        if (any_dead_threads && totally_free_length < idle_spawn_rate 
                && (!ps->pid               /* no process in the slot */
                    || ps->quiescing)) {   /* or at least one is going away */
            if (all_dead_threads) {
                /* great! we prefer these, because the new process can
                 * start more threads sooner.  So prioritize this slot 
                 * by putting it ahead of any slots with active threads.
                 *
                 * first, make room by moving a slot that's potentially still
                 * in use to the end of the array
                 */
                free_slots[free_length] = free_slots[totally_free_length];
                free_slots[totally_free_length++] = i;
            }
            else {
                /* slot is still in use - back of the bus
                 */
            free_slots[free_length] = i;
            }
            ++free_length;
        }
        /* XXX if (!ps->quiescing)     is probably more reliable  GLA */
        if (!any_dying_threads) {
            last_non_dead = i;
            ++total_non_dead;
        }
    }
    ap_max_daemons_limit = last_non_dead + 1;

    if (idle_thread_count > max_spare_threads) {
        /* Kill off one child */
        ap_mpm_pod_signal(pod, TRUE);
        idle_spawn_rate = 1;
    }
    else if (idle_thread_count < min_spare_threads) {
        /* terminate the free list */
        if (free_length == 0) {
            /* only report this condition once */
            static int reported = 0;
            
            if (!reported) {
                ap_log_error(APLOG_MARK, APLOG_ERR, 0, 
                             ap_server_conf,
                             "server reached MaxClients setting, consider"
                             " raising the MaxClients setting");
                reported = 1;
            }
            idle_spawn_rate = 1;
        }
        else {
            if (free_length > idle_spawn_rate) {
                free_length = idle_spawn_rate;
            }
            if (idle_spawn_rate >= 8) {
                ap_log_error(APLOG_MARK, APLOG_INFO, 0, 
                             ap_server_conf,
                             "server seems busy, (you may need "
                             "to increase StartServers, ThreadsPerChild "
                             "or Min/MaxSpareThreads), "
                             "spawning %d children, there are around %d idle "
                             "threads, and %d total children", free_length,
                             idle_thread_count, total_non_dead);
            }
            for (i = 0; i < free_length; ++i) {
                make_child(ap_server_conf, free_slots[i]);
            }
            /* the next time around we want to spawn twice as many if this
             * wasn't good enough, but not if we've just done a graceful
             */
            if (hold_off_on_exponential_spawning) {
                --hold_off_on_exponential_spawning;
            }
            else if (idle_spawn_rate < MAX_SPAWN_RATE) {
                idle_spawn_rate *= 2;
            }
        }
    }
    else {
      idle_spawn_rate = 1;
    }
}

static void server_main_loop(int remaining_children_to_start)
{
    int child_slot;
    apr_exit_why_e exitwhy;
    int status, processed_status;
    apr_proc_t pid;
    int i;

    while (!restart_pending && !shutdown_pending) {
        ap_wait_or_timeout(&exitwhy, &status, &pid, pconf);
        
        if (pid.pid != -1) {
            processed_status = ap_process_child_status(&pid, exitwhy, status);
            if (processed_status == APEXIT_CHILDFATAL) {
                shutdown_pending = 1;
                child_fatal = 1;
                return;
            }
            /* non-fatal death... note that it's gone in the scoreboard. */
            child_slot = find_child_by_pid(&pid);
            if (child_slot >= 0) {
                for (i = 0; i < ap_threads_per_child; i++)
                    ap_update_child_status_from_indexes(child_slot, i, SERVER_DEAD, 
                                                        (request_rec *) NULL);
                
                ap_scoreboard_image->parent[child_slot].pid = 0;
                ap_scoreboard_image->parent[child_slot].quiescing = 0;
                if (processed_status == APEXIT_CHILDSICK) {
                    /* resource shortage, minimize the fork rate */
                    idle_spawn_rate = 1;
                }
                else if (remaining_children_to_start
                    && child_slot < ap_daemons_limit) {
                    /* we're still doing a 1-for-1 replacement of dead
                     * children with new children
                     */
                    make_child(ap_server_conf, child_slot);
                    --remaining_children_to_start;
                }
#if APR_HAS_OTHER_CHILD
            }
            else if (apr_proc_other_child_read(&pid, status) == 0) {
                /* handled */
#endif
            }
            else if (is_graceful) {
                /* Great, we've probably just lost a slot in the
                 * scoreboard.  Somehow we don't know about this child.
                 */
                ap_log_error(APLOG_MARK, APLOG_WARNING, 0,
                             ap_server_conf,
                             "long lost child came home! (pid %ld)",
                             (long)pid.pid);
            }
            /* Don't perform idle maintenance when a child dies,
             * only do it when there's a timeout.  Remember only a
             * finite number of children can die, and it's pretty
             * pathological for a lot to die suddenly.
             */
            continue;
        }
        else if (remaining_children_to_start) {
            /* we hit a 1 second timeout in which none of the previous
             * generation of children needed to be reaped... so assume
             * they're all done, and pick up the slack if any is left.
             */
            startup_children(remaining_children_to_start);
            remaining_children_to_start = 0;
            /* In any event we really shouldn't do the code below because
             * few of the servers we just started are in the IDLE state
             * yet, so we'd mistakenly create an extra server.
             */
            continue;
        }

        perform_idle_server_maintenance();
    }
}

int ap_mpm_run(apr_pool_t *_pconf, apr_pool_t *plog, server_rec *s)
{
    int remaining_children_to_start;
    apr_status_t rv;

    ap_log_pid(pconf, ap_pid_fname);

    first_server_limit = server_limit;
    first_thread_limit = thread_limit;
    if (changed_limit_at_restart) {
        ap_log_error(APLOG_MARK, APLOG_WARNING, 0, s,
                     "WARNING: Attempt to change ServerLimit or ThreadLimit "
                     "ignored during restart");
        changed_limit_at_restart = 0;
    }
    
    /* Initialize cross-process accept lock */
    ap_lock_fname = apr_psprintf(_pconf, "%s.%" APR_PID_T_FMT,
                                 ap_server_root_relative(_pconf, ap_lock_fname),
                                 ap_my_pid);

    rv = apr_proc_mutex_create(&accept_mutex, ap_lock_fname, 
                               ap_accept_lock_mech, _pconf);
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_EMERG, rv, s,
                     "Couldn't create accept lock");
        mpm_state = AP_MPMQ_STOPPING;
        return 1;
    }

#if APR_USE_SYSVSEM_SERIALIZE
    if (ap_accept_lock_mech == APR_LOCK_DEFAULT || 
        ap_accept_lock_mech == APR_LOCK_SYSVSEM) {
#else
    if (ap_accept_lock_mech == APR_LOCK_SYSVSEM) {
#endif
        rv = unixd_set_proc_mutex_perms(accept_mutex);
        if (rv != APR_SUCCESS) {
            ap_log_error(APLOG_MARK, APLOG_EMERG, rv, s,
                         "Couldn't set permissions on cross-process lock; "
                         "check User and Group directives");
            mpm_state = AP_MPMQ_STOPPING;
            return 1;
        }
    }

    if (!is_graceful) {
        if (ap_run_pre_mpm(s->process->pool, SB_SHARED) != OK) {
            mpm_state = AP_MPMQ_STOPPING;
            return 1;
        }
        /* fix the generation number in the global score; we just got a new,
         * cleared scoreboard
         */
        ap_scoreboard_image->global->running_generation = ap_my_generation;
    }

    set_signals();
    /* Don't thrash... */
    if (max_spare_threads < min_spare_threads + ap_threads_per_child)
        max_spare_threads = min_spare_threads + ap_threads_per_child;

    /* If we're doing a graceful_restart then we're going to see a lot
     * of children exiting immediately when we get into the main loop
     * below (because we just sent them AP_SIG_GRACEFUL).  This happens pretty
     * rapidly... and for each one that exits we'll start a new one until
     * we reach at least daemons_min_free.  But we may be permitted to
     * start more than that, so we'll just keep track of how many we're
     * supposed to start up without the 1 second penalty between each fork.
     */
    remaining_children_to_start = ap_daemons_to_start;
    if (remaining_children_to_start > ap_daemons_limit) {
        remaining_children_to_start = ap_daemons_limit;
    }
    if (!is_graceful) {
        startup_children(remaining_children_to_start);
        remaining_children_to_start = 0;
    }
    else {
        /* give the system some time to recover before kicking into
            * exponential mode */
        hold_off_on_exponential_spawning = 10;
    }

    ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, ap_server_conf,
                "%s configured -- resuming normal operations",
                ap_get_server_version());
    ap_log_error(APLOG_MARK, APLOG_INFO, 0, ap_server_conf,
                "Server built: %s", ap_get_server_built());
#ifdef AP_MPM_WANT_SET_ACCEPT_LOCK_MECH
    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, ap_server_conf,
		"AcceptMutex: %s (default: %s)",
		apr_proc_mutex_name(accept_mutex),
		apr_proc_mutex_defname());
#endif
    restart_pending = shutdown_pending = 0;
    mpm_state = AP_MPMQ_RUNNING;

    server_main_loop(remaining_children_to_start);
    mpm_state = AP_MPMQ_STOPPING;

    if (shutdown_pending) {
        /* Time to gracefully shut down:
         * Kill child processes, tell them to call child_exit, etc...
         * (By "gracefully" we don't mean graceful in the same sense as 
         * "apachectl graceful" where we allow old connections to finish.)
         */
        ap_mpm_pod_killpg(pod, ap_daemons_limit, FALSE);
        ap_reclaim_child_processes(1);                /* Start with SIGTERM */

        if (!child_fatal) {
            /* cleanup pid file on normal shutdown */
            const char *pidfile = NULL;
            pidfile = ap_server_root_relative (pconf, ap_pid_fname);
            if ( pidfile != NULL && unlink(pidfile) == 0)
                ap_log_error(APLOG_MARK, APLOG_INFO, 0,
                             ap_server_conf,
                             "removed PID file %s (pid=%ld)",
                             pidfile, (long)getpid());
    
            ap_log_error(APLOG_MARK, APLOG_NOTICE, 0,
                         ap_server_conf, "caught SIGTERM, shutting down");
        }
        return 1;
    }

    /* we've been told to restart */
    apr_signal(SIGHUP, SIG_IGN);

    if (one_process) {
        /* not worth thinking about */
        return 1;
    }

    /* advance to the next generation */
    /* XXX: we really need to make sure this new generation number isn't in
     * use by any of the children.
     */
    ++ap_my_generation;
    ap_scoreboard_image->global->running_generation = ap_my_generation;
    
    if (is_graceful) {
        ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, ap_server_conf,
                     AP_SIG_GRACEFUL_STRING " received.  Doing graceful restart");
        /* wake up the children...time to die.  But we'll have more soon */
        ap_mpm_pod_killpg(pod, ap_daemons_limit, TRUE);
    

        /* This is mostly for debugging... so that we know what is still
         * gracefully dealing with existing request.
         */
        
    }
    else {
        /* Kill 'em all.  Since the child acts the same on the parents SIGTERM 
         * and a SIGHUP, we may as well use the same signal, because some user
         * pthreads are stealing signals from us left and right.
         */
        ap_mpm_pod_killpg(pod, ap_daemons_limit, FALSE);

        ap_reclaim_child_processes(1);                /* Start with SIGTERM */
        ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, ap_server_conf,
                    "SIGHUP received.  Attempting to restart");
    }

    return 0;
}

/* This really should be a post_config hook, but the error log is already
 * redirected by that point, so we need to do this in the open_logs phase.
 */
static int worker_open_logs(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
    apr_status_t rv;
    ap_listen_rec *lr;

    pconf = p;
    ap_server_conf = s;

    if ((num_listensocks = ap_setup_listeners(ap_server_conf)) < 1) {
        ap_log_error(APLOG_MARK, APLOG_ALERT|APLOG_STARTUP, 0,
                     NULL, "no listening sockets available, shutting down");
        return DONE;
    }

#if APR_O_NONBLOCK_INHERITED
    for(lr = ap_listeners ; lr != NULL ; lr = lr->next) {
        apr_socket_opt_set(lr->sd, APR_SO_NONBLOCK, 1);
    }
#endif /* APR_O_NONBLOCK_INHERITED */

    if (!one_process) {
        if ((rv = ap_mpm_pod_open(pconf, &pod))) {
            ap_log_error(APLOG_MARK, APLOG_CRIT|APLOG_STARTUP, rv, NULL,
                    "Could not open pipe-of-death.");
            return DONE;
        }
    }
    return OK;
}

static int worker_pre_config(apr_pool_t *pconf, apr_pool_t *plog, 
                             apr_pool_t *ptemp)
{
    static int restart_num = 0;
    int no_detach, debug, foreground;
    ap_directive_t *pdir;
    ap_directive_t *max_clients = NULL;
    apr_status_t rv;

    mpm_state = AP_MPMQ_STARTING;
    
    /* make sure that "ThreadsPerChild" gets set before "MaxClients" */
    for (pdir = ap_conftree; pdir != NULL; pdir = pdir->next) {
        if (strncasecmp(pdir->directive, "ThreadsPerChild", 15) == 0) {
            if (!max_clients) {
                break; /* we're in the clear, got ThreadsPerChild first */
            }
            else {
                /* now to swap the data */
                ap_directive_t temp;

                temp.directive = pdir->directive;
                temp.args = pdir->args;
                /* Make sure you don't change 'next', or you may get loops! */
                /* XXX: first_child, parent, and data can never be set
                 * for these directives, right? -aaron */
                temp.filename = pdir->filename;
                temp.line_num = pdir->line_num;

                pdir->directive = max_clients->directive;
                pdir->args = max_clients->args;
                pdir->filename = max_clients->filename;
                pdir->line_num = max_clients->line_num;
                
                max_clients->directive = temp.directive;
                max_clients->args = temp.args;
                max_clients->filename = temp.filename;
                max_clients->line_num = temp.line_num;
                break;
            }
        }
        else if (!max_clients
                 && strncasecmp(pdir->directive, "MaxClients", 10) == 0) {
            max_clients = pdir;
        }
    }

    debug = ap_exists_config_define("DEBUG");

    if (debug) {
        foreground = one_process = 1;
        no_detach = 0;
    }
    else {
        one_process = ap_exists_config_define("ONE_PROCESS");
        no_detach = ap_exists_config_define("NO_DETACH");
        foreground = ap_exists_config_define("FOREGROUND");
    }

    /* sigh, want this only the second time around */
    if (restart_num++ == 1) {
        is_graceful = 0;

        if (!one_process && !foreground) {
            rv = apr_proc_detach(no_detach ? APR_PROC_DETACH_FOREGROUND
                                           : APR_PROC_DETACH_DAEMONIZE);
            if (rv != APR_SUCCESS) {
                ap_log_error(APLOG_MARK, APLOG_CRIT, rv, NULL,
                             "apr_proc_detach failed");
                return HTTP_INTERNAL_SERVER_ERROR;
            }
        }
        parent_pid = ap_my_pid = getpid();
    }

    unixd_pre_config(ptemp);
    ap_listen_pre_config();
    ap_daemons_to_start = DEFAULT_START_DAEMON;
    min_spare_threads = DEFAULT_MIN_FREE_DAEMON * DEFAULT_THREADS_PER_CHILD;
    max_spare_threads = DEFAULT_MAX_FREE_DAEMON * DEFAULT_THREADS_PER_CHILD;
    ap_daemons_limit = server_limit;
    ap_threads_per_child = DEFAULT_THREADS_PER_CHILD;
    ap_pid_fname = DEFAULT_PIDLOG;
    ap_lock_fname = DEFAULT_LOCKFILE;
    ap_max_requests_per_child = DEFAULT_MAX_REQUESTS_PER_CHILD;
    ap_extended_status = 0;
#ifdef AP_MPM_WANT_SET_MAX_MEM_FREE
	ap_max_mem_free = APR_ALLOCATOR_MAX_FREE_UNLIMITED;
#endif

    apr_cpystrn(ap_coredump_dir, ap_server_root, sizeof(ap_coredump_dir));

    return OK;
}

static void threadpool_hooks(apr_pool_t *p)
{
    /* The worker open_logs phase must run before the core's, or stderr
     * will be redirected to a file, and the messages won't print to the
     * console.
     */
    static const char *const aszSucc[] = {"core.c", NULL};
    one_process = 0;

    ap_hook_open_logs(worker_open_logs, NULL, aszSucc, APR_HOOK_MIDDLE);
    /* we need to set the MPM state before other pre-config hooks use MPM query
     * to retrieve it, so register as REALLY_FIRST
     */
    ap_hook_pre_config(worker_pre_config, NULL, NULL, APR_HOOK_REALLY_FIRST);
}

static const char *set_daemons_to_start(cmd_parms *cmd, void *dummy,
                                        const char *arg) 
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }

    ap_daemons_to_start = atoi(arg);
    return NULL;
}

static const char *set_min_spare_threads(cmd_parms *cmd, void *dummy,
                                         const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }

    min_spare_threads = atoi(arg);
    if (min_spare_threads <= 0) {
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "WARNING: detected MinSpareThreads set to non-positive.");
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "Resetting to 1 to avoid almost certain Apache failure.");
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "Please read the documentation.");
       min_spare_threads = 1;
    }
       
    return NULL;
}

static const char *set_max_spare_threads(cmd_parms *cmd, void *dummy,
                                         const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }

    max_spare_threads = atoi(arg);
    return NULL;
}

static const char *set_max_clients (cmd_parms *cmd, void *dummy,
                                     const char *arg) 
{
    int max_clients;
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }

    /* It is ok to use ap_threads_per_child here because we are
     * sure that it gets set before MaxClients in the pre_config stage. */
    max_clients = atoi(arg);
    if (max_clients < ap_threads_per_child) {
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "WARNING: MaxClients (%d) must be at least as large",
                    max_clients);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " large as ThreadsPerChild (%d). Automatically",
                    ap_threads_per_child);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " increasing MaxClients to %d.",
                    ap_threads_per_child);
       max_clients = ap_threads_per_child;
    }
    ap_daemons_limit = max_clients / ap_threads_per_child;
    if ((max_clients > 0) && (max_clients % ap_threads_per_child)) {
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "WARNING: MaxClients (%d) is not an integer multiple",
                    max_clients);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " of ThreadsPerChild (%d), lowering MaxClients to %d",
                    ap_threads_per_child,
                    ap_daemons_limit * ap_threads_per_child);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " for a maximum of %d child processes,",
                    ap_daemons_limit);
       max_clients = ap_daemons_limit * ap_threads_per_child; 
    }
    if (ap_daemons_limit > server_limit) {
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "WARNING: MaxClients of %d would require %d servers,",
                    max_clients, ap_daemons_limit);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " and would exceed the ServerLimit value of %d.",
                    server_limit);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " Automatically lowering MaxClients to %d.  To increase,",
                    server_limit * ap_threads_per_child);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " please see the ServerLimit directive.");
       ap_daemons_limit = server_limit;
    } 
    else if (ap_daemons_limit < 1) {
        ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                     "WARNING: Require MaxClients > 0, setting to 1");
        ap_daemons_limit = 1;
    }
    return NULL;
}

static const char *set_threads_per_child (cmd_parms *cmd, void *dummy,
                                          const char *arg) 
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }

    ap_threads_per_child = atoi(arg);
    if (ap_threads_per_child > thread_limit) {
        ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                     "WARNING: ThreadsPerChild of %d exceeds ThreadLimit "
                     "value of %d", ap_threads_per_child,
                     thread_limit);
        ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                     "threads, lowering ThreadsPerChild to %d. To increase, please"
                     " see the", thread_limit);
        ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                     " ThreadLimit directive.");
        ap_threads_per_child = thread_limit;
    }
    else if (ap_threads_per_child < 1) {
        ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                     "WARNING: Require ThreadsPerChild > 0, setting to 1");
        ap_threads_per_child = 1;
    }
    return NULL;
}

static const char *set_server_limit (cmd_parms *cmd, void *dummy, const char *arg) 
{
    int tmp_server_limit;
    
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }

    tmp_server_limit = atoi(arg);
    /* you cannot change ServerLimit across a restart; ignore
     * any such attempts
     */
    if (first_server_limit &&
        tmp_server_limit != server_limit) {
        /* how do we log a message?  the error log is a bit bucket at this
         * point; we'll just have to set a flag so that ap_mpm_run()
         * logs a warning later
         */
        changed_limit_at_restart = 1;
        return NULL;
    }
    server_limit = tmp_server_limit;
    
    if (server_limit > MAX_SERVER_LIMIT) {
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "WARNING: ServerLimit of %d exceeds compile time limit "
                    "of %d servers,", server_limit, MAX_SERVER_LIMIT);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " lowering ServerLimit to %d.", MAX_SERVER_LIMIT);
       server_limit = MAX_SERVER_LIMIT;
    } 
    else if (server_limit < 1) {
	ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                     "WARNING: Require ServerLimit > 0, setting to 1");
	server_limit = 1;
    }
    return NULL;
}

static const char *set_thread_limit (cmd_parms *cmd, void *dummy, const char *arg) 
{
    int tmp_thread_limit;
    
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL) {
        return err;
    }

    tmp_thread_limit = atoi(arg);
    /* you cannot change ThreadLimit across a restart; ignore
     * any such attempts
     */
    if (first_thread_limit &&
        tmp_thread_limit != thread_limit) {
        /* how do we log a message?  the error log is a bit bucket at this
         * point; we'll just have to set a flag so that ap_mpm_run()
         * logs a warning later
         */
        changed_limit_at_restart = 1;
        return NULL;
    }
    thread_limit = tmp_thread_limit;
    
    if (thread_limit > MAX_THREAD_LIMIT) {
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    "WARNING: ThreadLimit of %d exceeds compile time limit "
                    "of %d servers,", thread_limit, MAX_THREAD_LIMIT);
       ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                    " lowering ThreadLimit to %d.", MAX_THREAD_LIMIT);
       thread_limit = MAX_THREAD_LIMIT;
    } 
    else if (thread_limit < 1) {
	ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                     "WARNING: Require ThreadLimit > 0, setting to 1");
	thread_limit = 1;
    }
    return NULL;
}

static const command_rec threadpool_cmds[] = {
UNIX_DAEMON_COMMANDS,
LISTEN_COMMANDS,
AP_INIT_TAKE1("StartServers", set_daemons_to_start, NULL, RSRC_CONF,
  "Number of child processes launched at server startup"),
AP_INIT_TAKE1("MinSpareThreads", set_min_spare_threads, NULL, RSRC_CONF,
  "Minimum number of idle children, to handle request spikes"),
AP_INIT_TAKE1("MaxSpareThreads", set_max_spare_threads, NULL, RSRC_CONF,
  "Maximum number of idle children"),
AP_INIT_TAKE1("MaxClients", set_max_clients, NULL, RSRC_CONF,
  "Maximum number of children alive at the same time"),
AP_INIT_TAKE1("ThreadsPerChild", set_threads_per_child, NULL, RSRC_CONF,
  "Number of threads each child creates"),
AP_INIT_TAKE1("ServerLimit", set_server_limit, NULL, RSRC_CONF,
  "Maximum value of MaxClients for this run of Apache"),
AP_INIT_TAKE1("ThreadLimit", set_thread_limit, NULL, RSRC_CONF,
  "Maximum worker threads in a server for this run of Apache"),
{ NULL }
};

module AP_MODULE_DECLARE_DATA mpm_threadpool_module = {
    MPM20_MODULE_STUFF,
    ap_mpm_rewrite_args,        /* hook to run before apache parses args */
    NULL,                       /* create per-directory config structure */
    NULL,                       /* merge per-directory config structures */
    NULL,                       /* create per-server config structure */
    NULL,                       /* merge per-server config structures */
    threadpool_cmds,            /* command apr_table_t */
    threadpool_hooks            /* register_hooks */
};

