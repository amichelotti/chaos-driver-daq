#!/bin/sh
## startup script for libera CU
# 8/2015 Andrea Michelotti
export LC_ALL="en_US.UTF-8"
CONFIG=/etc/default/libera-cu
if [ ! -f $CONFIG ];then
    echo "# libera configuration file \"$CONFIG\" missing"
    exit 1
fi

EXEC=/root/daqLiberaServer

case "$1" in
    start)
        # Start Libera CHAOS CU server
        echo -n "Starting Libera CHAOS CU server: $EXEC"
        start-stop-daemon --start --quiet -b --exec $EXEC -- --conf-file $CONFIG 2>&1 > /dev/null
        echo "."
        ;;

    stop)
        # Stop Libera CHAOS CU server
        echo -n "Stopping Libera CHAOS CU server: $EXEC"
        start-stop-daemon --stop --signal 9 --quiet --exec $EXEC
        echo "."
        ;;

    restart)
        $0 stop
        $0 start
        ;;

        *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac
