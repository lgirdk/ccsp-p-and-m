#!/bin/sh
##################################################################################
# If not stated otherwise in this file or this component's LICENSE  file the
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
###################################################################################

# This script will push the erouter0 IP address from ARM to ATOM.
#-----------------------------------------------------------------------------
if [ "$1" == "" ]; then
	exit 0;
fi

if [ -e "/usr/bin/rpcclient2" ]; then
	#for Puma6
	rpcclient2  "/usr/ccsp/wifi/sec_pushown.sh --ip4=\"$1\""
fi
