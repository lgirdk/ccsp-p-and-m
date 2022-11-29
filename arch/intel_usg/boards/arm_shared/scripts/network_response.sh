#!/bin/sh
#######################################################################################
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
#######################################################################################

# This script is used to check if the unit has got network access.
# Ping the DNS server in /etc/resolv.conf and check for response to ensure network access.
# If response to ping is received, write "204" response code in /var/tmp/networkresponse.txt as a
# workaround since that particular response code has been checked for webgui and other places.

# NOTE:
# Capturing the network response from google service (http://clients3.google.com/generate_204) 
# is replaced with ping to DNS server in /etc/resolv.conf as the network access test
# is dependent on the service by external party.

source /lib/rdk/t2Shared_api.sh

TRIGGER_STATE=$1
WAN_INTERFACE="erouter0"
REVERT_FLAG="/nvram/reverted"
EROUTER_IP=""
EROUTER_IP6=""
HAS_IP=0
RESPONSE="/var/tmp/networkresponse.txt"
ping_response=0
SERVER6_CONF="/etc/dibbler/server.conf"
responseCode=0
superResponse=0
gotResponse=0
v4Count=0
v6Count=0
v4retry_count=0
v6retry_count=0

export PATH=$PATH:/fss/gw
source /etc/utopia/service.d/log_capture_path.sh
BOX_TYPE=`cat /etc/device.properties | grep BOX_TYPE | cut -f 2 -d"="`
MODEL_NUM=`cat /etc/device.properties | grep MODEL_NUM  | cut -f2 -d=`

if [ "x$TRIGGER_STATE" != "xOnlyForNoRf" ]
then
if [ "$BOX_TYPE" = "XB3" ]; then
	LAST_REBOOT_REASON=`syscfg get X_RDKCENTRAL-COM_LastRebootReason`
	echo_t "Network Response: LastRebootReason is $LAST_REBOOT_REASON"
	if [ "$LAST_REBOOT_REASON" = "factory-reset" ]
	then
	    echo_t "Network Response: Removing onboard files from ATOM on factory reset"
		rpcclient2 "rm /nvram/.device_onboarded"
		rpcclient2 "rm /nvram/DISABLE_ONBOARD_LOGGING"
	fi
fi
fi

CAPTIVEPORTAL_ENABLED=`syscfg get CaptivePortal_Enable`
echo_t "Network Response : CaptivePortal enabled val is $CAPTIVEPORTAL_ENABLED"
recheckRevert=0

# Function to check if wan_fail_over is enabled or not
# return 1 if wan_fail_over is enabled
# return 0 if wan_fail_over is disabled
checkForWanFailOver()
{
	echo_t "Network Response: checkForWanFailOver"
	currentWanIf=`sysevent get current_wan_ifname`
	defaultWanIf=`sysevent get wan_ifname`
	if [ "$currentWanIf" == "$defaultWanIf" ]
	then
		echo_t "Network Response: checkForWanFailOver : disabled"
		return 0
	else
		#LTE wan interface is available
		echo_t "Network Response: checkForWanFailOver : enabled"
		return 1
	fi
}

# Function to check if RF CP should be shown
# return true if RF CP should be shown
# return false if RF CP shouldn't be shown
checkForRFCP()
{

if [ "$1" = "" ]
then
  # P&M up will make sure CM agent is up as well as
  # RFC values are picked
  echo_t "No RF CP: Check PAM initialized"
  PAM_UP=0
  while [ $PAM_UP -ne 1 ]
  do
    sleep 1
    #Check if CcspPandMSsp is up
    # PAM_PID=`pidof CcspPandMSsp`

    if [ -f "/tmp/pam_initialized" ]
    then
         PAM_UP=1
    fi
  done
  echo_t "RF CP: PAM is initialized" 
  enableRFCaptivePortal=`syscfg get enableRFCaptivePortal`
  if [ "$enableRFCaptivePortal" = "true" ]
  then
		ethWanEnabled=`syscfg get eth_wan_enabled`
        if [ "$ethWanEnabled" = "true" ]
        then
			noRfCp=0
			echo_t "Network Response: RF CP ethWan is enabled"
        else
			echo_t "Network Response: RF CP ethWan is disabled"
			checkForWanFailOver
			wfoStatus=$? 
			echo_t "Network Response: RF CP wan fail over status : $wfoStatus"
			if [ "$wfoStatus" = "0" ]
     		then
			 	echo_t "Network Response: RF CP wan_fail_over is disabled"
				RF_SIGNAL_STATUS=`dmcli eRT retv Device.DeviceInfo.X_RDKCENTRAL-COM_CableRfSignalStatus`
				if [ "$RF_SIGNAL_STATUS" = "false" ]
				then
					noRfCp=1
					echo_t "Network Response: RF CP wan_fail_over is disabled and RF is down"
				else
					noRfCp=0
					echo_t "Network Response: RF CP wan_fail_over is disabled and RF is up"
				fi
			else
				#LTE wan interface is available no need to bring no RF captive portal
				noRfCp=0
				echo_t "Network Response: RF CP wan_fail_over is enabled and bring down noRfCp"
			fi
		fi
  else
     echo_t "INFO:network_Response.sh - RF captive portal is disabled"
     noRfCp=0
  fi
  echo_t "Network Response: No RF values RF_SIGNAL_STATUS:$RF_SIGNAL_STATUS, noRfCp: $noRfCp" 
  if [ $noRfCp -eq 1 ]
  then
      syscfg set rf_captive_portal true
      syscfg commit
      return 1
  else
      syscfg set rf_captive_portal false
      syscfg commit
      return 0
  fi
elif [ "$1" = "recheck" ]
then
  # this argument is passed by the caller while in a loop 
  # to check RF signal. Hence no need to do initial checks.
  ethWanEnabled=`syscfg get eth_wan_enabled`
  if [ "$ethWanEnabled" = "true" ]
  then
      # Forcefully set RF signal status flag as true as eth wan mode would have
      # got enabled as part of auto WAN feature.
      echo_t "Network Response: RF CP recheck eth_wan_enabled is true"
      RF_SIGNAL_STATUS="true"
  else
		echo_t "Network Response: RF CP recheck eth_wan_enabled is false"
		checkForWanFailOver
		wfoStatus=$? 
		echo_t "Network Response: RF CP recheck wan fail over status : $wfoStatus"
		if [ "$wfoStatus" = "0" ]
		then
			echo_t "Network Response: RF CP recheck wan_fail_over is disabled"
			RF_SIGNAL_STATUS=`dmcli eRT retv Device.DeviceInfo.X_RDKCENTRAL-COM_CableRfSignalStatus`
		else
			#LTE wan interface is available no need to bring no RF captive portal
			# Forcefully set RF signal status flag as true
			echo_t "Network Response: RF CP recheck wan_fail_over is enabled"
			RF_SIGNAL_STATUS="true"
		fi 
  fi
  echo_t "Network Response: RF CP recheck RF_SIGNAL_STATUS: $RF_SIGNAL_STATUS"
  if [ "$RF_SIGNAL_STATUS" = "false" ]
  then
      dbValue=`syscfg get rf_captive_portal`
      if [ "$dbValue" = "false" ]
      then
          syscfg set rf_captive_portal true
          syscfg commit
      fi
      return 1 
  else
      dbValue=`syscfg get rf_captive_portal`
      if [ "$dbValue" = "true" ]
      then
          recheckRevert=1
          syscfg set rf_captive_portal false
          syscfg commit
      fi
      return 0
  fi
fi
}

rfCpInterface()
{
  #sysevent set rfcp started
  if [ "$CAPTIVEPORTAL_ENABLED" = "true" ]
  then
    checkForRFCP
    shouldRedirect=$? 
    echo_t "Network Response: Initial value of shouldRedirect is : $shouldRedirect"
     if [ "$shouldRedirect" = "1" ]
     then
        echo_t "Network Response: Restart events triggered for RF CP"
        /etc/redirect_url.sh rfcp &
        sleep 5
        while [ "$shouldRedirect" != "0" ]
        do
           checkForRFCP "recheck"
           shouldRedirect=$?
           echo_t "Network Response: shouldRedirect is : $shouldRedirect"
           sleep 5
        done
        echo_t "Network Response: Restart events triggered to come out from RF CP"
        /etc/revert_redirect.sh rfcp &
     fi
     needRevert=0 
     needRevert=`sysevent get norf_webgui`
     
     echo_t "Network Response : needRevert - $needRevert, recheckRevert - $recheckRevert" 
     if [ "$needRevert" = "1" ] || [ "$recheckRevert" = "1" ]
     then
        echo_t "Network Response: Restart services to come out from RF CP based on norf_webgui"
        /etc/revert_redirect.sh rfcp &
        sysevent set norf_webgui 0
     fi
  else
    syscfg set rf_captive_portal false
    syscfg commit
  fi
  #sysevent set rfcp completed
}

echo_t "Network Response: Device:$BOX_TYPE, TRIGGER_STATE:$TRIGGER_STATE"
if [ "$BOX_TYPE" = "XB6" ] || [ "$BOX_TYPE" = "MV2PLUS" ]
then
   echo_t "Network Response: Invoke rfCpInterface function"
   rfCpInterface
   if [ "x$TRIGGER_STATE" = "xOnlyForNoRf" ]
   then
       echo_t "Network Response: Exit from network response"
       exit 0
   fi
fi
echo_t "Network Response: Checking erouter0 ip address"

# This loop will wait till the erouter0 interface gets IP
while [ $HAS_IP -ne 1 ]
do
	sleep 2
	#Check if WAN got an IP
	EROUTER_IP=`ifconfig $WAN_INTERFACE | grep "inet addr" | cut -d":" -f2 | cut -d" " -f1`
        EROUTER_IP6=`ifconfig $WAN_INTERFACE | grep inet6 | grep Global`

	#If WAN has v4 just get out of the loop
	if [ "$EROUTER_IP" != "" ] || [ "$EROUTER_IP6" != "" ]
	then
		HAS_IP=1
	fi
	
done

restartLanServices()
{
    # Check if dnsmasq is up
    CHECK_DNSMASQ=`pidof dnsmasq`
    if [ "$CHECK_DNSMASQ" != "" ]
    then
        echo_t "Network Response: DHCP Server is already running , set an event to restart it"
        sysevent set dhcp_server-restart
    fi
	
    # Check if zebra is up
    CHECK_ZEBRA=`pidof zebra`
    if [ "$CHECK_ZEBRA" != "" ]
    then
        echo_t "Network Response: ZebraService is already running ,restarting it..."
        sysevent set zebra-restart
    fi
    
    echo_t "Network Response: Restarting v6Service"
    # Modify DNS server option in dibbler configuration
    if [ -e $SERVER6_CONF ]
    then
	Uncommented_line=`cat $SERVER6_CONF | grep dns-server | sed -e 's/.//'`
	#cat $SERVER6_CONF | grep dns-server | sed -e 's/.//' > $SERVER6_BKUP
	sed "/dns-server/c \
	\ \ \ \ $Uncommented_line"  $SERVER6_CONF > $SERVER6_BKUP
	cp -f $SERVER6_BKUP $SERVER6_CONF
	rm $SERVER6_BKUP
	cat $SERVER6_CONF
    else
       echo_t "No dibbler6 configuration available...."
    fi

    dibbler-server stop
    dibbler-server start

    sysevent set firewall-restart
    uptime=$(cut -d. -f1 /proc/uptime)
    if [ -e "/usr/bin/onboarding_log" ]; then
        /usr/bin/onboarding_log "RDKB_FIREWALL_RESTART:$uptime"
    fi
}

dns_ipv6=(`sysevent get ipv6_nameserver`)
dns_ipv4_count=$(sysevent get ipv4_${WAN_INTERFACE}_dns_number)
dns_ipv6_count=${#dns_ipv6[*]}

# Check if we are in captive portal mode.
# If unit is in captive portal mode, then restart all necessary services
# to get the corresponding configuration updated.
REDIRECTION_FLAG=`syscfg get redirection_flag`
echo_t "REDIRECTION_FLAG got is : $REDIRECTION_FLAG"
if [ "$REDIRECTION_FLAG" = "true" ] || [ "$BOX_TYPE" = "TCCBR" ] || [ "$BOX_TYPE" = "CFG3" ]
then
	# gotResponse will be marked 1 when any of the required responses are available.
	# We still have to depend on 204 response and response file /var/tmp/networkresponse.txt to minimize code changes
	# determining CP mode.
	# This loop will wait till we get a success response from any one of the URLs
	while [ $gotResponse -ne 1 ]
	do

		# Remove all response files if it already exists
		if [ -e $RESPONSE ] && [ -f "/tmp/.gotnetworkresponse" ]
		then
			responseCode=`cat $RESPONSE`
			rm -rf $RESPONSE				
		fi
	
	# RDKB-2752 : Comment out querying multiple URLs
	#if [ $gotResponse -eq 0 ] 
	#then
	#	curl -sL -w '%{http_code}\n' --interface $WAN_INTERFACE $URL_2 --connect-timeout 10 -m 10 -o /dev/null > $RESPONSE_2 

		echo_t "Network Response: Running ping to DNS server to check network access"

		if [ $responseCode -eq 204 ] && [ "$MODEL_NUM"  != "CGA4332COM" ]
		then
			echo_t "Network Response: Already $RESPONSE has been available with 204 response code."
			ping_response=$responseCode
		else
		   has_ipv4=`ifconfig $WAN_INTERFACE | grep "inet addr" | cut -d":" -f2 | cut -d" " -f1`
		   has_ipv6=`ifconfig $WAN_INTERFACE | grep inet6 | grep Global`

		   if [ "$has_ipv4" != "" ] && [ $v4Count -lt $dns_ipv4_count ];
		   then
				dns_ipv4=$(sysevent get ipv4_${WAN_INTERFACE}_dns_${v4Count})
				ping_response=`ping -4 -I $WAN_INTERFACE -c 1 -W 5 ${dns_ipv4} &> /dev/null && echo "204" || echo "0"`
				v4Count=$(($v4Count + 1))

				if [ $ping_response -eq 0 ] && [ $v4Count -eq $dns_ipv4_count ] && [ $v4retry_count -lt 2 ];
				then
					v4Count=0;
					v4retry_count=$(($v4retry_count + 1))
				fi
		   elif [ "$has_ipv6" != "" ] && [ $v6Count -lt $dns_ipv6_count ];
		   then
				ping_response=`ping -6 -I $WAN_INTERFACE -c 1 -W 5 ${dns_ipv6[$v6Count]} &> /dev/null && echo "204" || echo "0"`
				v6Count=$(($v6Count + 1))

				if [ $ping_response -eq 0 ] && [ $v6Count -eq $dns_ipv6_count ] && [ $v6retry_count -lt 2 ];
				then
					v6Count=0;
					v6retry_count=$(($v6retry_count + 1))
				fi
		   else
				echo_t "Network Response: Ping using IPv6 or IPv4 to dns server doesn't get reply. Exit."
				exit 0
		   fi

		fi
        
		# Touching this file and will be checked from webgui.sh
		touch /tmp/.gotnetworkresponse
		echo_t "Network Response: Response code received is $ping_response"

		if [ $ping_response -eq 204 ]
		then
			echo_t "Network Response: Ping to the dns server is successful."
			gotResponse=1
		else
			echo_t "Network Response: Ping to the dns server failed."

			if [ -f "/etc/ONBOARD_LOGGING_ENABLE" ] && [ ! -e "/nvram/.device_onboarded" ]; then
				echo_t "Network Response: Enabling onboard logs backup"
				touch /tmp/backup_onboardlogs
			fi
		fi
	
		if [ $gotResponse -eq 1 ] 
		then
			#superResponse=204
			echo 204 > $RESPONSE
			echo_t "Network Response: Got 204. Move on.."

			#if [ "$BOX_TYPE" = "XB6" ] || [ "$BOX_TYPE" = "MV2PLUS" ]; then
			#	white=0
			#	solid=0
			#	/usr/bin/SetLED $white $solid		#Set LED to Solid White
			#fi
 
                        if [ "$MODEL_NUM" = "CGA4332COM" ] || [ "$BOX_TYPE" = "XB6" ]
                        then
			        echo_t "Network Response: Setting Online LED to solid white .."
				white=0
				solid=0
				/usr/bin/SetLED $white $solid		#Set LED to Solid White
			fi

			isUnitActivated=`syscfg get unit_activated`
			if [ "$isUnitActivated" = "" ]
			then
			    echo_t "Network Response: unit_activated is NULL. Retry"
			    isUnitActivated=`syscfg get unit_activated`
			    if [ "$isUnitActivated" = "" ]
			    then
			       echo_t "Network Response: unit_activated is NULL after retry. Fallback to 0"
			       isUnitActivated=0
			    fi
			fi

			if [ $isUnitActivated -eq 0 ]
			then
				# Set syscfg parameter to indicate unit is activated
				syscfg set unit_activated 1
				syscfg commit
				uptime=$(cut -d. -f1 /proc/uptime)
				echo_t "Exit_Captive_Mode:$uptime"
				t2ValNotify "btime_cpexit_split" $uptime
				if [ -e "/usr/bin/onboarding_log" ]; then
					/usr/bin/onboarding_log "Exit_Captive_Mode:$uptime"
				fi
				echo_t "Network Response: Restart DHCP server"
				sysevent set dhcp_server-restart
				echo_t "Network Response: Restart Firewall"
				sysevent set firewall-restart
				uptime=$(cut -d. -f1 /proc/uptime)
				if [ -e "/usr/bin/onboarding_log" ]; then
					/usr/bin/onboarding_log "RDKB_FIREWALL_RESTART:$uptime"
				fi
				# We will flag that DHCP server is restarted after activation
				# This event is checked in redirect_url.sh
				echo_t "Network Response: Set sysevent to indicate dhcp started"
				sysevent set dhcp_after_activation flagged
			fi

			if [ -e "$REVERT_FLAG" ]
			then
				echo_t "Network Response: Reverted flag should not be present in case of default state"	
				rm -f $REVERT_FLAG
			fi
			break;
		else
				echo_t "Network Response: Didnt recieve success response..should retry.."

    			if [ "$BOX_TYPE" = "XB6" ] || [ "$BOX_TYPE" = "MV2PLUS" ]; then
        			white=0
        			blink=1
					interval=1
        			/usr/bin/SetLED $white $blink $interval     #Set LED to Blink White per second
    			fi

                        if [ "$MODEL_NUM" = "CGA4332COM" ]; then
        			white=0
        			blink=1
				interval=5
        			/usr/bin/SetLED $white $blink $interval     #Set LED to Blink White 5x per second
    			fi

				unitActivated=`syscfg get unit_activated`
				if [ "$unitActivated" != 0 ]
				then
				    # As we haven't received 204 response, indicate unit is not activated
				    syscfg set unit_activated 0
				    syscfg commit
				    uptime=$(cut -d. -f1 /proc/uptime)
				    echo_t "Enter_Captive_Mode:$uptime"
				    t2ValNotify "btime_cpenter_split" $uptime
				    if [ -e "/usr/bin/onboarding_log" ]; then
				        /usr/bin/onboarding_log "Enter_Captive_Mode:$uptime"
				    fi
				    # Remove /var/tmp/networkresponse.txt file.
				    # LAN services should not be in captive portal configuration. 
				    # LAN services depend on 204 response to configure captive portal.
				    # By default, we have put 204 into /var/tmp/networkresponse.txt from utopia_init.sh
				    echo_t "Network Response: Removing networkresponse.txt file before restarting services.."
				    rm -rf $RESPONSE
				    restartLanServices
				fi
                        
		fi

	done
else
	# We are here as Db value indicate that the unit is already configured with personalized
	# WiFi settings. So we mark the device as activated rather querying any service.
	echo_t "Network Response: WiFi is already configured. Hence marking as got response 204"
	echo 204 > $RESPONSE
	#Set syscfg parameter to indicate unit is activated
	syscfg set unit_activated 1
	syscfg commit

    if [ "$BOX_TYPE" = "XB6" ] || [ "$BOX_TYPE" = "MV2PLUS" ]; then
#        white=0
#        solid=0
#        /usr/bin/SetLED $white $solid       #Set LED to Solid White
         # RDKB-38922: Check network status to confirm unit is with proper bootfile.
         # Based on response received, LED behavior should be altered to blinkng white or solid white
         /etc/network_led_check.sh &
         echo_t "LED status script triggered"
    fi

	if [ ! -e "$REVERT_FLAG" ]
	then
		echo_t "Network Response: WiFi is already configured. Set reverted flag in nvram"	
		touch $REVERT_FLAG
	fi
fi

# to update device onboarding state.
if [ -f "/etc/ONBOARD_LOGGING_ENABLE" ]; then
    NETWORKRESPONSEVALUE=`cat /var/tmp/networkresponse.txt`
    unitActivated=`syscfg get unit_activated`
    echo_t "Network Response: Updating device onboarding state"
    if [ "$unitActivated" = "1" ] && [ "$NETWORKRESPONSEVALUE" = "204" ]
    then
        echo_t "Network Response: Device is detected as onboarded"
		if [ -f "/nvram/.device_onboarded" ]
		then
			echo_t "Network Response: /nvram/.device_onboarded is already present"
		else
		    echo_t "Network Response: Creating /nvram/.device_onboarded file"
		    touch /nvram/.device_onboarded
		    if [ -e "/usr/bin/rpcclient2" ] && [ "$BOX_TYPE" == "XB3" ]; then
		        rpcclient2 "/bin/touch /nvram/.device_onboarded"
		    fi
		fi
    else
        echo_t "Network Response: Device is detected as not onboarded"
        if [ -f "/nvram/.device_onboarded" ]
        then
            rm -rf /nvram/.device_onboarded
            if [ -e "/usr/bin/rpcclient2" ] && [ "$BOX_TYPE" == "XB3" ]; then
                rpcclient2 "rm /nvram/.device_onboarded"
            fi
        fi
    fi
fi

# In factory default condition, we should check if all LAN configurations are up.
# If they are up, restart it.
# This check is needed because when WAN comes up later than LAN we should make all 
# configurations in sync

CAPTIVEPORTAL_ENABLED=`syscfg get CaptivePortal_Enable`
echo_t "Network Response : CaptivePortal enabled val is $CAPTIVEPORTAL_ENABLED"


if [ "$REDIRECTION_FLAG" = "true" ] && [ "$CAPTIVEPORTAL_ENABLED" == "true" ] 
then
        #Check if lighttpd daemon is up
        CHECK_LIGHTTPD=`pidof lighttpd`
        if [ "$CHECK_LIGHTTPD" != "" ]
        then
                echo_t "Network Response: Check ConfigureWiFi parameter is in sync or not"
                SET_CONFIGURE_FLAG=`psmcli get eRT.com.cisco.spvtg.ccsp.Device.WiFi.NotifyWiFiChanges`

                #Read the http response value
                NETWORKRESPONSEVALUE=`cat /var/tmp/networkresponse.txt`

                iter=0
                max_iter=2
                while [ "$SET_CONFIGURE_FLAG" = "" ] && [ "$iter" -le $max_iter ]
                do
                   iter=$((iter+1))
                   echo_t "$iter"
                   SET_CONFIGURE_FLAG=`psmcli get eRT.com.cisco.spvtg.ccsp.Device.WiFi.NotifyWiFiChanges`
                done
                echo_t "Network Response: NotifyWiFiChanges is $SET_CONFIGURE_FLAG"
                echo_t "Network Response: Redirection_flag value is $REDIRECTION_FLAG"

                if [ "$NETWORKRESPONSEVALUE" = "204" ] && [ "$SET_CONFIGURE_FLAG" = "true" ]
                then
                    count=0
                    MaxCount=30
                    while : ; do

                        if [ -f "/tmp/pam_initialized" ]
                        then
                           if [ ! -f "/tmp/.configurewifidone" ]
                           then
                              echo_t "Network Response: WiFi is not configured,setting ConfigureWiFi to true"
                              output=`dmcli eRT setvalues Device.DeviceInfo.X_RDKCENTRAL-COM_ConfigureWiFi bool TRUE`
                              check_success=`echo $output | grep  "Execution succeed."`
                              if [ "$check_success" != "" ]
                              then
                                 echo_t "Network Response : Setting ConfigureWiFi to true is success"
				uptime=$(cut -d. -f1 /proc/uptime)
				 echo_t "Enter_WiFi_Personalization_captive_mode:$uptime"
				 t2ValNotify "btime_wcpenter_split" $uptime
				 if [ -e "/usr/bin/onboarding_log" ]; then
				   /usr/bin/onboarding_log "Enter_WiFi_Personalization_captive_mode:$uptime"
				 fi
                                 touch /tmp/.configurewifidone
                              fi
                           fi
                           break
                        else
                           sleep 5
                           count=$((count+1))
                           if [ "$count" -ge "$MaxCount" ]
                           then
                              echo_t "Network Response : Max wait for PandM reached , breaking the loop"
                              break
                           fi
                        fi
                    done
                else
                    echo_t "Network Response: WiFi is already personalized setting redirection_flag to false"
                    #We reached here as redirection_flag is "true". But WiFi is configured already as per notification status.
                    #Set syscfg value to false now.
                    syscfg set redirection_flag false
                    syscfg commit
                    echo_t "Network Response: ConfigureWiFi is false.Set reverted flag in nvram"
                    if [ ! -e "$REVERT_FLAG" ]
                    then
                       touch $REVERT_FLAG
                    fi
                fi

        fi
fi

