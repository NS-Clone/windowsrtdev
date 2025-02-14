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

#include "test_apr.h"
#include "apr_file_info.h"
#include "apr_errno.h"
#include "apr_pools.h"
#include "apr_tables.h"

#if defined(WIN32) || defined(NETWARE) || defined(OS2)
#define PSEP ";"
#define DSEP "\\"
#else
#define PSEP ":"
#define DSEP "/"
#endif

#define PX ""
#define P1 "first path"
#define P2 "second" DSEP "path"
#define P3 "th ird" DSEP "path"
#define P4 "fourth" DSEP "pa th"
#define P5 "fifthpath"

static const char *parts_in[] = { P1, P2, P3, PX, P4, P5 };
static const char *path_in = P1 PSEP P2 PSEP P3 PSEP PX PSEP P4 PSEP P5;
static const int  parts_in_count = sizeof(parts_in)/sizeof(*parts_in);

static const char *parts_out[] = { P1, P2, P3, P4, P5 };
static const char *path_out = P1 PSEP P2 PSEP P3 PSEP P4 PSEP P5;
static const int  parts_out_count = sizeof(parts_out)/sizeof(*parts_out);

static void list_split_multi(CuTest *tc)
{
    int i;
    apr_status_t rv;
    apr_array_header_t *pathelts;

    pathelts = NULL;
    rv = apr_filepath_list_split(&pathelts, path_in, p);
    CuAssertPtrNotNull(tc, pathelts);
    CuAssertIntEquals(tc, APR_SUCCESS, rv);
    CuAssertIntEquals(tc, parts_out_count, pathelts->nelts);
    for (i = 0; i < pathelts->nelts; ++i)
        CuAssertStrEquals(tc, parts_out[i], ((char**)pathelts->elts)[i]);
}

static void list_split_single(CuTest *tc)
{
    int i;
    apr_status_t rv;
    apr_array_header_t *pathelts;

    for (i = 0; i < parts_in_count; ++i)
    {
        pathelts = NULL;
        rv = apr_filepath_list_split(&pathelts, parts_in[i], p);
        CuAssertPtrNotNull(tc, pathelts);
        CuAssertIntEquals(tc, APR_SUCCESS, rv);
        if (parts_in[i][0] == '\0')
            CuAssertIntEquals(tc, 0, pathelts->nelts);
        else
        {
            CuAssertIntEquals(tc, 1, pathelts->nelts);
            CuAssertStrEquals(tc, parts_in[i], *(char**)pathelts->elts);
        }
    }
}

static void list_merge_multi(CuTest *tc)
{
    int i;
    char *liststr;
    apr_status_t rv;
    apr_array_header_t *pathelts;

    pathelts = apr_array_make(p, parts_in_count, sizeof(const char*));
    for (i = 0; i < parts_in_count; ++i)
        *(const char**)apr_array_push(pathelts) = parts_in[i];

    liststr = NULL;
    rv = apr_filepath_list_merge(&liststr, pathelts, p);
    CuAssertPtrNotNull(tc, liststr);
    CuAssertIntEquals(tc, APR_SUCCESS, rv);
    CuAssertStrEquals(tc, liststr, path_out);
}

static void list_merge_single(CuTest *tc)
{
    int i;
    char *liststr;
    apr_status_t rv;
    apr_array_header_t *pathelts;

    pathelts = apr_array_make(p, 1, sizeof(const char*));
    apr_array_push(pathelts);
    for (i = 0; i < parts_in_count; ++i)
    {
        *(const char**)pathelts->elts = parts_in[i];
        liststr = NULL;
        rv = apr_filepath_list_merge(&liststr, pathelts, p);
        if (parts_in[i][0] == '\0')
            CuAssertPtrEquals(tc, NULL, liststr);
        else
        {
            CuAssertPtrNotNull(tc, liststr);
            CuAssertIntEquals(tc, APR_SUCCESS, rv);
            CuAssertStrEquals(tc, liststr, parts_in[i]);
        }
    }
}


CuSuite *testpath(void)
{
    CuSuite *suite = CuSuiteNew("Path lists");

    SUITE_ADD_TEST(suite, list_split_multi);
    SUITE_ADD_TEST(suite, list_split_single);
    SUITE_ADD_TEST(suite, list_merge_multi);
    SUITE_ADD_TEST(suite, list_merge_single);

    return suite;
}

