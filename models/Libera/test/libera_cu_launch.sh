#!/bin/sh
EXEC=/root/daqLiberaServer
if [ -z "$1" ];then
echo "## you must define the libera name, like LIBERA02,LIBERA01.."
exit 1
fi

if $EXEC --unit-server-alias $1 --metadata-server 192.168.150.31:5000 --log-on-file --log-level debug --log-file $1.log 2&>1 >/dev/null &  then
    echo "launching OK."
else
    echo "## cannot launch daqLiberaServer"
    exit 1
fi
sleep 1
if ps -fe |grep $EXEC |grep -v;then
    echo "ok."
    exit 0
fi
echo "# $EXEC exited"
exit 1

