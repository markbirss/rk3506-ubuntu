#!/bin/sh

case "$1" in
	start)
		if [ -f /sys/class/rfkill/rfkill0/state ]; then
			echo 1 > /sys/class/rfkill/rfkill0/state
		fi
		;;
	*)
		echo "Usage: [start|stop|restart]" >&2
		exit 3
		;;
esac
