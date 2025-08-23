#!/bin/bash

RESULT_DIR=/userdata/rockchip-test/
RESULT_LOG=${RESULT_DIR}/suspend_resume.txt
SUSPEND_TIME=60  # Fixed suspend time to 60 seconds (1 minute)
WAKE_DELAY=5     # Delay after wake-up before next action
MAX_CYCLES=10000

# Enable Debug
# echo N > /sys/module/printk/parameters/console_suspend
# echo 1 > /sys/power/pm_print_times

# Check if RTC is present
if [ ! -e "/sys/class/rtc/rtc0/wakealarm" ]; then
    echo "RTC not present, please check if RTC is enabled"
    exit 1
fi


# Function to generate random number
# random() {
#   hexdump -n 2 -e '/2 "%u"' /dev/urandom
# }

# Create result directory
mkdir -p ${RESULT_DIR}


auto_suspend_resume_rtc() {
    cnt=0

    # Sync system time to RTC
    hwclock --systohc
    echo "$(date): auto_suspend_resume_rtc starts" > ${RESULT_LOG}

    while [ $cnt -lt $MAX_CYCLES ]; do
        echo "Completed $cnt suspend/resume cycles"

        # Set new wake time 1 minute into the future
        echo 0 > /sys/class/rtc/rtc0/wakealarm
        echo "+${SUSPEND_TIME}" > /sys/class/rtc/rtc0/wakealarm
        pm-suspend

	echo "Waking for $WAKE_DELAY seconds"
        # System wakes up here
        sleep $WAKE_DELAY  # Wait for 5 seconds after wake-up

        echo "$(date): Cycle: $cnt - Sleep: $SUSPEND_TIME Wake Delay: $WAKE_DELAY" >> ${RESULT_LOG}

        cnt=$((cnt + 1))
    done
}

auto_suspend_resume_rtc
