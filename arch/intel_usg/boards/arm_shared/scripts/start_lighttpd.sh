#!/bin/sh

LOCAL_UI_ENABLE="$(syscfg get local_ui_enable)"

# Fixme: Calling the init script only works for non-systemd builds...
#        Need to add support for starting via systemd too.

lighttpd_start()
{
	LIGHTTPD_PID=$(pidof lighttpd)

	if [ -n "${LIGHTTPD_PID}" ]
	then
		/bin/kill ${LIGHTTPD_PID}
	fi

	CAPTIVE_PORTAL_ENABLE=$(syscfg get CaptivePortal_Enable)

	if [ "$(syscfg get cloud_enable_flag)" = "1" ]
	then
		sed 's|^server\.document-root .*|server.document-root = "/www/cloud/"|' < /etc/lighttpd/lighttpd.conf > /var/lighttpd.conf
		CONF_FILE="/var/lighttpd.conf"

	elif [ "$CAPTIVE_PORTAL_ENABLE" = "true" ]
	then
		cp /etc/lighttpd/lighttpd.conf /var/lighttpd.conf
		CONF_FILE="/var/lighttpd.conf"

	else
		CONF_FILE="/etc/lighttpd/lighttpd.conf"
	fi

	if [ "$CONF_FILE" = "/var/lighttpd.conf" ]
	then
		if [ "$CAPTIVE_PORTAL_ENABLE" = "true" ]
		then
			LAN_IP=$(syscfg get lan_ipaddr)

			{
			echo
			echo '$HTTP["scheme"] == "http" {'
			echo '  $HTTP["host"] == "'"$LAN_IP"'" {'
			echo '    $HTTP["url"] =~ "'"$LAN_IP"'" {'
			echo '    }'
			echo '  }'
			echo '  else $HTTP["url"] !~ "'"$LAN_IP"'" {'
			echo '    url.redirect = ("" => "http://'"$LAN_IP"'/")'
			echo '    url.redirect-code = 302'
			echo '  }'
			echo '}'
			} >> $CONF_FILE
		fi
	fi

	if [ "${LOCAL_UI_ENABLE}" = "true" ]
	then
		/usr/sbin/lighttpd -f ${CONF_FILE}
	fi
}

case "$1"
in
	start)
		lighttpd_start
		;;

	stop)
		/etc/init.d/lighttpd stop
		;;

	restart)
		/etc/init.d/lighttpd stop
		if [ "${LOCAL_UI_ENABLE}" = "true" ]
		then
			lighttpd_start
		fi
		;;
esac
