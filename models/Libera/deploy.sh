#!/bin/bash
if [ -z "$1" ];then
    echo "## you must specify target branch"
    exit 1
fi
wget https://opensource.lnf.infn.it/binary/chaos/$1/arm/chaos-distrib-$1-build_arm_linux26.tar.gz
tar xvfz chaos-distrib-$1-build_arm_linux26.tar.gz chaos-distrib/bin/daqLiberaServer
scp  chaos-distrib/bin/daqLiberaServer michelo@192.168.143.252:/export/chaos-libera/old/
echo "* removing binary chaos-distrib-$1-build_arm_linux26.tar.gz"
rm -rf chaos-distrib-$1-build_arm_linux26.tar.gz chaos-distrib

