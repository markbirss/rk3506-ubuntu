#!/bin/sh

EVENT=${1:-short-press}

LONG_PRESS_TIMEOUT=3 # s
DEBOUNCE=2 # s
PIDFILE="/tmp/$(basename $0).pid"
LOCKFILE=/tmp/.power_key

log()
{
	logger -t $(basename $0) "[$$]: $@"
}

parse_wake_time()
{
	RK_SUSPEND_STATE=/sys/kernel/wakeup_reasons/last_suspend_time
	if ! [ -f "$RK_SUSPEND_STATE" ]; then
		return -1
	fi

	SLEEP_TIME=$(sed 's/ /+/' "$RK_SUSPEND_STATE" | bc)
	if [ "$SLEEP_TIME" = "0" ]; then
		log "We have not slept before..."
		return -1
	fi

	LAST_SUSPEND="$(stat -c "%Y" /sys/power/state)"
	LAST_RESUME="$(echo "$LAST_SUSPEND+$SLEEP_TIME" | bc | cut -d'.' -f1)"
	NOW="$(date "+%s")"
	WAKE_TIME="$(( "$NOW" - "$LAST_RESUME" ))"

	if [ "$WAKE_TIME" -lt 0 ]; then
		log "Something is wrong, time changed?"
		return -1
	fi

	log "Last resume: $(date -d "@$LAST_RESUME" "+%D %T")..."
}

short_press()
{
	log "Power key short press..."

	if which systemctl >/dev/null; then
		SUSPEND_CMD="systemctl suspend"
	elif which pm-suspend >/dev/null; then
		SUSPEND_CMD="pm-suspend"
	else
		SUSPEND_CMD="echo -n mem > /sys/power/state"
	fi

	# Debounce
	if [ -f $LOCKFILE ]; then
		log "Too close to the latest request..."
		return 0
	fi

	if parse_wake_time; then
		if [ "$WAKE_TIME" -le $DEBOUNCE ]; then
			log "We are just resumed!"
			return 0
		fi
	fi

	log "Prepare to suspend..."

	touch $LOCKFILE
	sh -c "$SUSPEND_CMD"
	{ sleep $DEBOUNCE && rm $LOCKFILE; }&
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
		exec 3<$0
		flock -x 3

		start-stop-daemon -K -q -p $PIDFILE || true
		start-stop-daemon -S -q -b -m -p $PIDFILE -x /bin/sh -- \
			-c "sleep $LONG_PRESS_TIMEOUT; $0 long-press"

		# Unlock
		flock -u 3
		;;
	release)
		# Avoid race with press event
		sleep .5

		start-stop-daemon -K -q -p $PIDFILE && short_press
		;;
	short-press)
		short_press
		;;
	long-press)
		long_press
		;;
esac
