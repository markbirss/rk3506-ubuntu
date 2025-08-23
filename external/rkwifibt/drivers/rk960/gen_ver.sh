#!/bin/bash

svn_submiter=$(svn info | grep "Last Changed Author: " | sed -e "s/Last Changed Author: //g")
svn_last_rev=$(svn info | grep "Last Changed Rev: " | sed -e "s/Last Changed Rev: //g")
svn_date=$(svn info | grep "Last Changed Date: " | sed -e "s/Last Changed Date: //g")

complie_version="svn"$svn_last_rev"."$svn_submiter" "$svn_date

echo $complie_version

sed -i "/^#define *RK960_VERSION */s/\(#define *RK960_VERSION *\)\(\".*\"\)/\1\"$complie_version\"/" rk960.h
