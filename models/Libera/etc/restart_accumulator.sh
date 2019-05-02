#!/bin/bash
lista="libera01 libera02 libera03 libera07 libera08 libera09"
# libera12 libera13
for i in $lista;do
    echo "* stopping $i"
    ssh root@$i /etc/init.d/chaos-us.sh stop    
    echo "* remount rw $i"
    ssh root@$i "mount -o remount ,rw /"
    echo "* remount chaous"
    ssh root@$i "mount /mnt/chaos"
    echo "copy ntp servers config"
    scp -r openntpd/ root@$i:/etc
    echo "sync time $i"
    ssh root@$i "ntpd -s"
    sleep 1
    echo "* starting  chaos $i"
    ssh root@$i /etc/init.d/chaos-us.sh start
done

