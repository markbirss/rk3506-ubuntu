#!/bin/bash
#

stop()
{
    ps -a | grep cyclictest -w && killall -9 cyclictest
    ps -a | grep amp_monitor -w && killall -9 amp_monitor
    ps -a | grep stress-ng -w && killall -9 stress-ng
}

start()
{
    #/rockchip-test/ddr/memtester_test.sh
    stress-ng -c 2 --cpu-load 100 --io 4 --vm 2 --vm-bytes 256M &
    amp_monitor &
    sleep 2
    taskset -c 2 cyclictest -c0 -m -t1 -p99 -h 300 -N -D 1H &
}

case "$1" in
    restart)
        stop
        start
        ;;
    start)
        start
        ;;
    stop)
        stop
        ;;
    *)
		echo "Usage: $0 {start|stop|restart}"
        exit 1
esac

exit 0
