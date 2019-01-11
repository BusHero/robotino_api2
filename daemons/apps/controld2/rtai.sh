#!/bin/sh

check_insmod()
{
        cd /usr/realtime/modules
        for var in $*
        do
                if [ ! `lsmod | grep -o ^$var` ]
                then
                        insmod $var.ko
                        echo "$var loaded"
                fi
        done
}

check_rmmod()
{
        for var in $*
        do
                if [ `lsmod | grep -o ^$var` ]
                then
                        rmmod $var
                        echo "$var unloaded"
                fi
        done
}

case "$1" in
    start)
                check_insmod rtai_hal rtai_lxrt rtai_sem rtai_serial
        ;;

    stop)
                check_rmmod rtai_serial rtai_sem rtai_lxrt rtai_hal
        ;;

    restart)
                check_rmmod rtai_serial rtai_sem rtai_lxrt rtai_hal
                check_insmod rtai_hal rtai_lxrt rtai_sem rtai_serial
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac
