/*
 * Copyright (C) 2023 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __RECOVERY_VERSION_H__
#define __RECOVERY_VERSION_H__

#include "recovery_autogenerate.h"
/* define in makefile
 * GIT_COMMIT_INFO
 * major.minor.revision-gX
 * major:main version number, increase when there are significant changes
 * minor:minor version number, increase when there is an increase in functionality or features
 * revision:revised version number, increase when there are bug fixes or minor modifications
*/
#define PLAIN_VERSION   "V1.0.1"
#define _CONS(str1,str2)    str1 #str2
#define CONS(A,B)           _CONS(A,B)
#define RECOVERY_VERSION_STRING CONS(PLAIN_VERSION, GIT_COMMIT_INFO)

#endif  /* __RECOVERY_VERSION_H__ */