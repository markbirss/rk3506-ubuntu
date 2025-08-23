#!/bin/sh
#
# Hook scripts for suspend/resume.
#
# pm-utils: /usr/lib/pm-utils/sleep.d/03wifibt
# system-sleep: /lib/systemd/system-sleep/03wifibt

. "${PM_FUNCTIONS:-/}"

case "$1" in
	pre|hibernate|suspend) wifibt-init.sh suspend ;;
	post|thaw|resume) wifibt-init.sh resume ;;
	*) exit $NA ;;
esac
