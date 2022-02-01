#!/bin/bash
##################################################################################
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:

#  Copyright 2018 RDK Management

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

source /etc/device.properties

BINPATH="/usr/bin"
if [ -f /tmp/cp_subsys_ert ]; then
   Subsys="eRT."
elif [ -e ./cp_subsys_emg ]; then
   Subsys="eMG."
else
   Subsys=""
fi

stopTr069()
{
  if [ "$BOX_TYPE" = "XB3" ]; then
     TR69_PID=`pidof CcspTr069PaSsp`
     if [ "$TR69_PID" != "" ]; then
         killall CcspTr069PaSsp
     fi
  else
     systemctl stop CcspTr069PaSsp.service
  fi
}

startTr069()
{
  if [ "$BOX_TYPE" = "XB3" ]; then
     TR69_PID=`pidof CcspTr069PaSsp`
     if [ "$TR69_PID" = "" ]; then
        cd /usr/ccsp/tr069pa
        $BINPATH/CcspTr069PaSsp -subsys $Subsys
     fi
  else
     systemctl start CcspTr069PaSsp.service
  fi
}

if [ "$1" = "enable" ]; then
    startTr069
elif [ "$1" = "disable" ]; then
    stopTr069
fi
