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

#include "apr_private.h"
#include "apr_thread_proc.h"
#include "apr_file_io.h"

#ifndef THREAD_PROC_H
#define THREAD_PROC_H

#define SHELL_PATH "cmd.exe"

struct apr_thread_t {
    apr_pool_t *pool;
    HANDLE td;
    apr_int32_t cancel;
    apr_int32_t cancel_how;
    void *data;
    apr_thread_start_t func;
    apr_status_t exitval;
};

struct apr_threadattr_t {
    apr_pool_t *pool;
    apr_int32_t detach;
    apr_size_t stacksize;
};

struct apr_threadkey_t {
    apr_pool_t *pool;
    DWORD key;
};

struct apr_procattr_t {
    apr_pool_t *pool;
    apr_file_t *parent_in;
    apr_file_t *child_in;
    apr_file_t *parent_out;
    apr_file_t *child_out;
    apr_file_t *parent_err;
    apr_file_t *child_err;
    char *currdir;
    apr_int32_t cmdtype;
    apr_int32_t detached;
    apr_child_errfn_t *errfn;
    apr_int32_t errchk;
};

struct apr_thread_once_t {
    long value;
};

extern apr_status_t apr_threadproc_init(apr_pool_t *pool);

#endif  /* ! THREAD_PROC_H */

