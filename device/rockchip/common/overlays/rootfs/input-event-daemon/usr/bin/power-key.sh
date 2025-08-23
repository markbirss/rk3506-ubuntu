#!/bin/sh

EVENT=${1:-short-press}

LONG_PRESS_TIMEOUT=3 # s
DEBOUNCE=2 # s
PID_FILE=/var/run/power_key.pid
LOCK_FILE=/var/run/.power_key.lock

log()
{
        if which logger >/dev/null; then
		logger -t $(basename $0) "[$$]: $@"
	fi
        echo "$(basename $0): $@"
}

check_suspend_state()
{
	LAST_MODIFY="$(stat -c "%Y" /sys/power/state)"
	NOW="$(date "+%s")"
	WAKE_TIME="$(expr "$NOW" - "$LAST_MODIFY")"

	log "Last state changed: $(date -d "@$LAST_MODIFY" "+%D %T")..."

	if [ "$WAKE_TIME" -le $DEBOUNCE ]; then
		log "Too close to the latest suspending request..."
		return 1
	fi
}

# HACK: Monitoring power state changes and update the suspended state
while ! ls -l /proc/*/exe 2>/dev/null | grep -wq inotifywait; do
	inotifywait -e modify /sys/power/state >/dev/null 2>&1

	# Avoid race with freezing processes
	sleep .2
	touch /sys/power/state
done&

short_press()
{
	log "Power key short press..."

	if ! check_suspend_state; then
		log "Do nothing!"
		return 0
	fi

	if which systemctl >/dev/null; then
		SUSPEND_CMD="systemctl suspend"
	elif which pm-suspend >/dev/null; then
		SUSPEND_CMD="pm-suspend"
	else
		SUSPEND_CMD="echo -n mem > /sys/power/state"
	fi

	log "Prepare to suspend..."
	touch /sys/power/state
	sh -c "$SUSPEND_CMD"
	touch /sys/power/state
}

long_press()
{
	log "Power key long press (${LONG_PRESS_TIMEOUT}s)..."

	log "Prepare to power off..."

	poweroff
}

log "Received power key event: $@..."

case "$EVENT" in
	press)
		# Lock it
		touch $LOCK_FILE
		exec 3<$LOCK_FILE
		flock -x 3

		start-stop-daemon -K -q -p $PID_FILE || true
		start-stop-daemon -S -q -b -m -p $PID_FILE -x /bin/sh -- \
			-c "sleep $LONG_PRESS_TIMEOUT; $0 long-press"

		# Unlock
		flock -u 3
		;;
	release)
		# Avoid race with press event
		sleep .5

		start-stop-daemon -K -q -p $PID_FILE && short_press
		;;
	short-press)
		short_press
		;;
	long-press)
		long_press
		;;
esac
