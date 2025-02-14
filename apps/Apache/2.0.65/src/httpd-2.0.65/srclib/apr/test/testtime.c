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

#include "apr_time.h"
#include "apr_errno.h"
#include "apr_general.h"
#include "apr_lib.h"
#include "test_apr.h"
#include "apr_strings.h"
#include <time.h>

#define STR_SIZE 45

/* The time value is used throughout the tests, so just make this a global.
 * Also, we need a single value that we can test for the positive tests, so
 * I chose the number below, it corresponds to:
 *           2002-08-14 12:05:36.186711 -25200 [257 Sat].
 * Which happens to be when I wrote the new tests.
 */
static apr_time_t now = APR_INT64_C(1032030336186711);

static char* print_time (apr_pool_t *pool, const apr_time_exp_t *xt)
{
    return apr_psprintf (pool,
                         "%04d-%02d-%02d %02d:%02d:%02d.%06d %+05d [%d %s]%s",
                         xt->tm_year + 1900,
                         xt->tm_mon,
                         xt->tm_mday,
                         xt->tm_hour,
                         xt->tm_min,
                         xt->tm_sec,
                         xt->tm_usec,
                         xt->tm_gmtoff,
                         xt->tm_yday + 1,
                         apr_day_snames[xt->tm_wday],
                         (xt->tm_isdst ? " DST" : ""));
}


static void test_now(CuTest *tc)
{
    apr_time_t timediff;
    apr_time_t current;
    time_t os_now;

    current = apr_time_now();
    time(&os_now);

    timediff = os_now - (current / APR_USEC_PER_SEC); 
    /* Even though these are called so close together, there is the chance
     * that the time will be slightly off, so accept anything between -1 and
     * 1 second.
     */
    CuAssert(tc, "apr_time and OS time do not agree", 
             (timediff > -2) && (timediff < 2));
}

static void test_gmtstr(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;

    rv = apr_time_exp_gmt(&xt, now);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_time_exp_gmt");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    CuAssertStrEquals(tc, "2002-08-14 19:05:36.186711 +0000 [257 Sat]", 
                      print_time(p, &xt));
}

static void test_exp_lt(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    time_t posix_secs = (time_t)apr_time_sec(now);
    struct tm *posix_exp = localtime(&posix_secs);

    rv = apr_time_exp_lt(&xt, now);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_time_exp_lt");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);

#define CHK_FIELD(f) \
    CuAssert(tc, "Mismatch in " #f, posix_exp->f == xt.f)

    CHK_FIELD(tm_sec);
    CHK_FIELD(tm_min);
    CHK_FIELD(tm_hour);
    CHK_FIELD(tm_mday);
    CHK_FIELD(tm_mon);
    CHK_FIELD(tm_year);
    CHK_FIELD(tm_wday);
    CHK_FIELD(tm_yday);
    CHK_FIELD(tm_isdst);
#undef CHK_FIELD
}

static void test_exp_get_gmt(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    apr_time_t imp;
    apr_int64_t hr_off_64;

    rv = apr_time_exp_gmt(&xt, now);
    CuAssertTrue(tc, rv == APR_SUCCESS);
    rv = apr_time_exp_get(&imp, &xt);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_time_exp_get");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    hr_off_64 = (apr_int64_t) xt.tm_gmtoff * APR_USEC_PER_SEC;
    CuAssertTrue(tc, now + hr_off_64 == imp);
}

static void test_exp_get_lt(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    apr_time_t imp;
    apr_int64_t hr_off_64;

    rv = apr_time_exp_lt(&xt, now);
    CuAssertTrue(tc, rv == APR_SUCCESS);
    rv = apr_time_exp_get(&imp, &xt);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_time_exp_get");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    hr_off_64 = (apr_int64_t) xt.tm_gmtoff * APR_USEC_PER_SEC;
    CuAssertTrue(tc, now + hr_off_64 == imp);
}

static void test_imp_gmt(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    apr_time_t imp;

    rv = apr_time_exp_gmt(&xt, now);
    CuAssertTrue(tc, rv == APR_SUCCESS);
    rv = apr_time_exp_gmt_get(&imp, &xt);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_time_exp_gmt_get");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    CuAssertTrue(tc, now == imp);
}

static void test_rfcstr(CuTest *tc)
{
    apr_status_t rv;
    char str[STR_SIZE];

    rv = apr_rfc822_date(str, now);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_rfc822_date");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    CuAssertStrEquals(tc, "Sat, 14 Sep 2002 19:05:36 GMT", str);
}

static void test_ctime(CuTest *tc)
{
    apr_status_t rv;
    char apr_str[STR_SIZE];
    char libc_str[STR_SIZE];
    time_t posix_sec = (time_t)apr_time_sec(now);

    rv = apr_ctime(apr_str, now);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_ctime");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    strcpy(libc_str, ctime(&posix_sec));
    *strchr(libc_str, '\n') = '\0';

    CuAssertStrEquals(tc, libc_str, apr_str);
}

static void test_strftime(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    char *str = NULL;
    apr_size_t sz;

    rv = apr_time_exp_gmt(&xt, now);
    str = apr_palloc(p, STR_SIZE + 1);
    rv = apr_strftime(str, &sz, STR_SIZE, "%R %A %d %B %Y", &xt);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_strftime");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    CuAssertStrEquals(tc, "19:05 Saturday 14 September 2002", str);
}

static void test_strftimesmall(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    char str[STR_SIZE];
    apr_size_t sz;

    rv = apr_time_exp_gmt(&xt, now);
    rv = apr_strftime(str, &sz, STR_SIZE, "%T", &xt);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_strftime");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    CuAssertStrEquals(tc, "19:05:36", str);
}

static void test_exp_tz(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    apr_int32_t hr_off = -5 * 3600; /* 5 hours in seconds */

    rv = apr_time_exp_tz(&xt, now, hr_off);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_time_exp_tz");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
    CuAssertTrue(tc, (xt.tm_usec == 186711) && 
                     (xt.tm_sec == 36) &&
                     (xt.tm_min == 5) && 
                     (xt.tm_hour == 14) &&
                     (xt.tm_mday == 14) &&
                     (xt.tm_mon == 8) &&
                     (xt.tm_year == 102) &&
                     (xt.tm_wday == 6) &&
                     (xt.tm_yday == 256));
}

static void test_strftimeoffset(CuTest *tc)
{
    apr_status_t rv;
    apr_time_exp_t xt;
    char str[STR_SIZE];
    apr_size_t sz;
    apr_int32_t hr_off = -5 * 3600; /* 5 hours in seconds */

    apr_time_exp_tz(&xt, now, hr_off);
    rv = apr_strftime(str, &sz, STR_SIZE, "%T", &xt);
    if (rv == APR_ENOTIMPL) {
        CuNotImpl(tc, "apr_strftime");
    }
    CuAssertTrue(tc, rv == APR_SUCCESS);
}

/* 0.9.4 and earlier rejected valid dates in 2038 */
static void test_2038(CuTest *tc)
{
    apr_time_exp_t xt;
    apr_time_t t;

    /* 2038-01-19T03:14:07.000000Z */
    xt.tm_year = 138;
    xt.tm_mon = 0;
    xt.tm_mday = 19;
    xt.tm_hour = 3;
    xt.tm_min = 14;
    xt.tm_sec = 7;
    
    apr_assert_success(tc, "explode January 19th, 2038",
                       apr_time_exp_get(&t, &xt));
}

CuSuite *testtime(void)
{
    CuSuite *suite = CuSuiteNew("Time");

    SUITE_ADD_TEST(suite, test_now);
    SUITE_ADD_TEST(suite, test_gmtstr);
    SUITE_ADD_TEST(suite, test_exp_lt);
    SUITE_ADD_TEST(suite, test_exp_get_gmt);
    SUITE_ADD_TEST(suite, test_exp_get_lt);
    SUITE_ADD_TEST(suite, test_imp_gmt);
    SUITE_ADD_TEST(suite, test_rfcstr);
    SUITE_ADD_TEST(suite, test_ctime);
    SUITE_ADD_TEST(suite, test_strftime);
    SUITE_ADD_TEST(suite, test_strftimesmall);
    SUITE_ADD_TEST(suite, test_exp_tz);
    SUITE_ADD_TEST(suite, test_strftimeoffset);
    SUITE_ADD_TEST(suite, test_2038);

    return suite;
}

