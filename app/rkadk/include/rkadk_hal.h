/*
 * Copyright (c) 2023 Rockchip, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __RKADK_HAL_H__
#define __RKADK_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OS_RTT
#include "../../../libc/posix/ipc/semaphore.h"
#include "../../../libc/posix/io/poll/poll.h"
/* define in  <unistd.h> for other gcc compiler*/
/* Values for the second argument to access.
   These may be OR'd together.  */
#define  R_OK  4    /* Test for read permission.  */
#define  W_OK  2    /* Test for write permission.  */
#define  X_OK  1    /* Test for execute permission.  */
#define  F_OK  0    /* Test for existence.  */

/* define in  <ipc.h> for other gcc compiler*/
/* Mode bits for `msgget', `semget', and `shmget'.  */
#define IPC_CREAT   01000    /* Create key if key does not exist. */
#define IPC_EXCL    02000    /* Fail if key exists.  */
#define IPC_NOWAIT  04000    /* Return error on wait.  */

/* Control commands for `msgctl', `semctl', and `shmctl'.  */
#define IPC_RMID   0    /* Remove identifier.  */
#define IPC_SET    1    /* Set `ipc_perm' options.  */
#define IPC_STAT   2    /* Get `ipc_perm' options.  */
#ifdef __USE_GNU
# define IPC_INFO  3    /* See ipcs.  */
#endif

#else
#include <byteswap.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <sys/unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sys/poll.h>
#endif

#ifdef __cplusplus
}
#endif
#endif