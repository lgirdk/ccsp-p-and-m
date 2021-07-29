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

	if [ "$(syscfg get cloud_enable_flag)" = "1" ]
	then
		if [ ! -f /var/lighttpd.conf ]
		then
			sed 's|^server\.document-root .*|server.document-root = "/www/cloud/"|' < /etc/lighttpd/lighttpd.conf > /var/lighttpd.conf
			ln -sf /etc/lighttpd/gateway.conf /var/gateway.conf
		fi
		CONF_FILE="/var/lighttpd.conf"
	else
		CONF_FILE="/etc/lighttpd/lighttpd.conf"
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
