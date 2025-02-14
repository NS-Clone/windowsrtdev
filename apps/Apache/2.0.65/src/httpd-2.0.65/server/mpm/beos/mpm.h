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

#ifndef APACHE_MPM_BEOS_H
#define APACHE_MPM_BEOS_H

#define BEOS_MPM
#include "scoreboard.h"

#define MPM_NAME "Beos"
#define MPM_CHILD_PID(i) (ap_scoreboard_image->servers[0][i].tid)
#define MPM_NOTE_CHILD_KILLED(i) (MPM_CHILD_PID(i) = 0)

#define AP_MPM_WANT_RECLAIM_CHILD_PROCESSES
#define AP_MPM_WANT_WAIT_OR_TIMEOUT
#define AP_MPM_WANT_PROCESS_CHILD_STATUS
#define AP_MPM_WANT_SET_PIDFILE
#define AP_MPM_WANT_SET_SCOREBOARD
#define AP_MPM_WANT_SET_MAX_REQUESTS
#define AP_MPM_WANT_SET_COREDUMPDIR
#define AP_MPM_WANT_SET_MAX_MEM_FREE

extern int ap_max_child_assigned;
extern server_rec *ap_server_conf;
extern int ap_threads_per_child;

#endif /* APACHE_MPM_BEOS_H */
