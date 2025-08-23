#!/bin/bash

set_cpu_freq() {
    local policy_path=$1
    local target_freq=$2
    echo userspace > "${policy_path}/scaling_governor"
    echo "${target_freq}" > "${policy_path}/scaling_setspeed"
    local cur=$(cat "${policy_path}/scaling_cur_freq")
    local min=$(cat "${policy_path}/scaling_min_freq")
    if [[ "$cur" -eq "$target_freq" ]] || [[ "$cur" -le "$min" ]]; then
        echo "CPU freq policy:${policy_path##*/} successfully changed to ${cur} KHz"
    else
        echo "Failed to change CPU freq to ${target_freq} KHz, current freq: ${cur} KHz"
        exit 1
    fi
}

cycle_frequencies() {
    local end_time=$(( $(date +%s) + 86400 )) # 24 hours from now
    local cnt=0
    while [[ $(date +%s) -lt $end_time ]]; do
        for policy_path in /sys/devices/system/cpu/cpufreq/policy*; do
            read -ra freqs <<< $(cat "${policy_path}/scaling_available_frequencies")
            local freq=${freqs[RANDOM % ${#freqs[@]}]}
            echo -n "Cycle: $cnt, "
            set_cpu_freq "$policy_path" "$freq"
            ((cnt++))
        done
        sleep 10 # Sleep for 10 seconds before changing frequencies again
    done
    echo "24-hour CPU frequency cycle test completed."
}

# Check for user input to set a specific frequency or cycle frequencies
if [[ "$#" -eq 1 ]]; then
    set_cpu_freq "/sys/devices/system/cpu/cpufreq/policy0" "$1"
else
    cycle_frequencies
fi
