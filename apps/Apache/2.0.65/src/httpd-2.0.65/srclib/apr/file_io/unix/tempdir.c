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
#include "apr_file_io.h"
#include "apr_strings.h"
#include "apr_env.h"

/*
 * FIXME
 * Currently, this variable is a bit of misnomer.
 * The intention is to have this in APR's global pool so that we don't have 
 * to go through this every time.
 */
static char global_temp_dir[APR_PATH_MAX+1] = { 0 };

/* Try to open a temporary file in the temporary dir, write to it,
   and then close it. */
static int test_tempdir(const char *temp_dir, apr_pool_t *p)
{
    apr_file_t *dummy_file;
    const char *path = apr_pstrcat(p, temp_dir, "/apr-tmp.XXXXXX", NULL);

    if (apr_file_mktemp(&dummy_file, (char *)path, 0, p) == APR_SUCCESS) {
        if (apr_file_putc('!', dummy_file) == APR_SUCCESS) {
            if (apr_file_close(dummy_file) == APR_SUCCESS) {
                return 1;
            }
        }
    }
    return 0;
}


APR_DECLARE(apr_status_t) apr_temp_dir_get(const char **temp_dir, 
                                           apr_pool_t *p)
{
    apr_status_t apr_err;
    const char *try_dirs[] = { "/tmp", "/usr/tmp", "/var/tmp" };
    const char *try_envs[] = { "TMP", "TEMP", "TMPDIR" };
    char *cwd;
    int i;

    /* Our goal is to find a temporary directory suitable for writing
       into.  We'll only pay the price once if we're successful -- we
       cache our successful find.  Here's the order in which we'll try
       various paths:

          $TMP
          $TEMP
          $TMPDIR
          "C:\TEMP"     (windows only)
          "SYS:\TMP"    (netware only)
          "/tmp"
          "/var/tmp"
          "/usr/tmp"
          P_tmpdir      (POSIX define)
          `pwd` 

       NOTE: This algorithm is basically the same one used by Python
       2.2's tempfile.py module.  */

    /* Try the environment first. */
    for (i = 0; i < (sizeof(try_envs) / sizeof(const char *)); i++) {
        char *value;
        apr_err = apr_env_get(&value, try_envs[i], p);
        if ((apr_err == APR_SUCCESS) && value) {
            apr_size_t len = strlen(value);
            if (len && (len < APR_PATH_MAX) && test_tempdir(value, p)) {
                memcpy(global_temp_dir, value, len + 1);
                goto end;
            }
        }
    }

#ifdef WIN32
    /* Next, on Win32, try the C:\TEMP directory. */
    if (test_tempdir("C:\\TEMP", p)) {
        memcpy(global_temp_dir, "C:\\TEMP", 7 + 1);
        goto end;
    }
#endif
#ifdef NETWARE
    /* Next, on NetWare, try the SYS:/TMP directory. */
    if (test_tempdir("SYS:/TMP", p)) {
        memcpy(global_temp_dir, "SYS:/TMP", 8 + 1);
        goto end;
    }
#endif

    /* Next, try a set of hard-coded paths. */
    for (i = 0; i < (sizeof(try_dirs) / sizeof(const char *)); i++) {
        if (test_tempdir(try_dirs[i], p)) {
            memcpy(global_temp_dir, try_dirs[i], strlen(try_dirs[i]) + 1);
            goto end;
        }
    }

#ifdef P_tmpdir
    /* 
     * If we have it, use the POSIX definition of where 
     * the tmpdir should be 
     */
    if (test_tempdir(P_tmpdir, p)) {
        memcpy(global_temp_dir, P_tmpdir, strlen(P_tmpdir) +1);
        goto end;
    }
#endif
    
    /* Finally, try the current working directory. */
    if (APR_SUCCESS == apr_filepath_get(&cwd, APR_FILEPATH_NATIVE, p)) {
        if (test_tempdir(cwd, p)) {
            memcpy(global_temp_dir, cwd, strlen(cwd) + 1);
            goto end;
        }
    }

end:
    if (global_temp_dir[0]) {
        *temp_dir = apr_pstrdup(p, global_temp_dir);
        return APR_SUCCESS;
    }
    return APR_EGENERAL;
}
