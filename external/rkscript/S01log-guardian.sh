#!/bin/sh
### BEGIN INIT INFO
# Provides:       log-guardian
# Required-Start:
# Required-Stop:
# Default-Start:  S
# Default-Stop:
# Description:    Truncate log files when no space left on device
### END INIT INFO

case "$1" in
	start|restart|reload|force-reload|"") log-guardian& ;;
	stop) log-guardian --quit ;;
	status)
		# No-op
		;;
	*)
		echo "Usage: start" >&2
		exit 3
		;;
esac

:
