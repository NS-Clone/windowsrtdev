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

#include "apr_general.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_apr.h"

static void rand_exists(CuTest *tc)
{
#if !APR_HAS_RANDOM
    CuNotImpl(tc, "apr_generate_random_bytes");
#else
    unsigned char c[42];

    /* There must be a better way to test random-ness, but I don't know
     * what it is right now.
     */
    apr_assert_success(tc, "apr_generate_random_bytes failed",
                       apr_generate_random_bytes(c, sizeof c));
#endif
}    

CuSuite *testrand(void)
{
    CuSuite *suite = CuSuiteNew("Random");

    SUITE_ADD_TEST(suite, rand_exists);

    return suite;
}

