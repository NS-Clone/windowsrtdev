/* Must include apr.h first so that we can undefine
    the standard prototypes macros after it messes with
    them. */
#include "apr.h"

#undef APR_DECLARE
#undef APR_DECLARE_NONSTD
#undef APR_DECLARE_HOOK
#undef APR_POOL_DECLARE_ACCESSOR
#undef APR_DECLARE_DATA

/* Preprocess all of the standard APR headers. */
#include "apr_allocator.h"
#include "apr_atomic.h"
#include "apr_compat.h"
#include "apr_dso.h"
#include "apr_env.h"
#include "apr_errno.h"
#include "apr_file_info.h"
#include "apr_file_io.h"
#include "apr_fnmatch.h"
#include "apr_general.h"
#include "apr_getopt.h"
#include "apr_global_mutex.h"
#include "apr_hash.h"
#include "apr_inherit.h"
#include "apr_lib.h"
#include "apr_mmap.h"
#include "apr_network_io.h"
#include "apr_poll.h"
#include "apr_pools.h"
#include "apr_portable.h"
#include "apr_proc_mutex.h"
#include "apr_ring.h"
#include "apr_shm.h"
#include "apr_signal.h"
#include "apr_strings.h"
#include "apr_support.h"
#include "apr_tables.h"
#include "apr_thread_cond.h"
#include "apr_thread_mutex.h"
#include "apr_thread_proc.h"
#include "apr_thread_rwlock.h"
#include "apr_time.h"
#include "apr_user.h"
#include "apr_version.h"
#include "apr_want.h"


/* Must include apu.h first so that we can undefine
    the standard prototypes macros after it messes with
    them. */
#include "apu.h"

#undef APU_DECLARE
#undef APU_DECLARE_NONSTD
#undef APU_DECLARE_DATA

/* Preprocess all of the standard APR headers. */
#include "apr_anylock.h"
#include "apr_base64.h"
#include "apr_buckets.h"
#include "apr_date.h"
#include "apr_dbm.h"
#include "apr_hooks.h"
#include "apr_md4.h"
#include "apr_md5.h"
#include "apr_optional.h"
#include "apr_optional_hooks.h"
#include "apr_queue.h"
#include "apr_reslist.h"
#include "apr_rmm.h"
#include "apr_sdbm.h"
#include "apr_sha1.h"
#include "apr_strmatch.h"
#include "apr_uri.h"
#include "apr_uuid.h"
#include "apr_xlate.h"
#include "apr_xml.h"
#include "apu_compat.h"
#include "apu_version.h"
#include "apu_want.h"
