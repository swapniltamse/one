#!/bin/bash

# -------------------------------------------------------------------------- #
# Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             #
#                                                                            #
# Licensed under the Apache License, Version 2.0 (the "License"); you may    #
# not use this file except in compliance with the License. You may obtain    #
# a copy of the License at                                                   #
#                                                                            #
# http://www.apache.org/licenses/LICENSE-2.0                                 #
#                                                                            #
# Unless required by applicable law or agreed to in writing, software        #
# distributed under the License is distributed on an "AS IS" BASIS,          #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
# See the License for the specific language governing permissions and        #
# limitations under the License.                                             #
#--------------------------------------------------------------------------- #

if [ -z "${ONE_LOCATION}" ]; then
    TMCOMMON=/usr/lib/one/mads/tm_common.sh
else
    TMCOMMON=$ONE_LOCATION/lib/mads/tm_common.sh
fi

. $TMCOMMON

SIZE=$1
FSTYPE=$2
DST=$3

DST_PATH=`arg_path $DST`
DST_HOST=`arg_host $DST`
DST_DIR=`dirname $DST_PATH`

exec_and_log "$SSH $DST_HOST mkdir -p $DST_DIR" \
    "Error creating directory $DST_DIR"
exec_and_log "$SSH $DST_HOST $DD if=/dev/zero of=$DST_PATH bs=1 count=1 seek=${SIZE}M" \
    "Could not create image $DST_PATH"
exec_and_log "$SSH $DST_HOST $MKFS -t $FSTYPE -F $DST_PATH" \
    "Unable to create filesystem $FSTYPE in $DST_PATH"
exec_and_log "$SSH $DST_HOST chmod a+rw $DST_PATH"