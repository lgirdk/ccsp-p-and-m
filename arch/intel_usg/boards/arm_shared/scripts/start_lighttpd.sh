#!/bin/sh

LOCAL_UI_ENABLE="$(syscfg get local_ui_enable)"

# Fixme: Calling the init script only works for non-systemd builds...
#        Need to add support for starting via systemd too.

case "$1"
in
	start)
		if [ "${LOCAL_UI_ENABLE}" = "true" ] ; then
			/etc/init.d/lighttpd start
		fi
		;;

	stop)
		/etc/init.d/lighttpd stop
		;;

	restart)
		if [ "${LOCAL_UI_ENABLE}" = "true" ] ; then
			/etc/init.d/lighttpd restart
		else
			/etc/init.d/lighttpd stop
		fi
		;;

esac
