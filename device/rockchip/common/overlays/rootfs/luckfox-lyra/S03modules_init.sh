#!/bin/sh
#
# Modules init
#


start() {
	find /lib/modules/$(uname -r)/kernel/ -name "*.ko" | xargs -I {} sh -c 'modprobe $(basename {})'
}

stop() {
	find /lib/modules/$(uname -r)/kernel/ -name "*.ko" | xargs -I {} sh -c 'modprobe -r $(basename {})'
}

case "$1" in
	start)
		start
		;;
	#stop)
	#	stop
	#	;;
	*)
		echo "Usage: $0 {start|stop|restart}"
		exit 1
esac

exit $?
