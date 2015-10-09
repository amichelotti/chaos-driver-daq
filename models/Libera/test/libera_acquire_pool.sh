#!/bin/sh

usage(){
    echo "$0 <acq type [0:stop,1:dd,2:SA]> <acquisition loops>"

}
if [ -z "$1" ];then
    echo "## You must specify the acquisition type"
    usage
    exit 1
fi
if [ -z "$2" ];then
    echo "## You must specify the number of acquisitions"
    usage
    exit 1
fi
daqLiberaClient -d LIBERA01/LIBERA_ACQUIRE0 -d LIBERA02/LIBERA_ACQUIRE0 -d LIBERA03/LIBERA_ACQUIRE0 -d LIBERA07/LIBERA_ACQUIRE0 -d LIBERA08/LIBERA_ACQUIRE0 -d LIBERA09/LIBERA_ACQUIRE0 --acquire $1 --loops $2 --log-on-file --log-level debug --metadata-server chaost-mds1.chaos.lnf.infn.it:5000
