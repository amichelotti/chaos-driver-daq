#!/bin/bash
lista="libera05 libera06 libera10 libera04 libera02 libera08 libera13 libera12  libera03 libera07  libera09 libera01"
for i in $lista;do
    echo "* remount rw $i"
    ssh root@$i "mount -o remount ,rw /"
    echo "* remount rw"
    ssh root@$i "mount /mnt/chaos"
    echo "copy ntp servers config"
    scp -r openntpd/ root@$i:/etc
    echo "sync time $i"
    ssh root@$i "ntpd -s"
    ssh root@$i /etc/init.d/chaos-us.sh stop
    sleep 1
     echo "* starting  chaos $i"
     ssh root@$i /etc/init.d/chaos-us.sh start
done

for i in $lista;do
echo "* connecting to $i"
ssh root@$i /etc/init.d/chaos-us.sh stop
done
 sleep 6
 for i in $lista;do
 echo "* connecting to $i"
 ssh root@$i /etc/init.d/chaos-us.sh start
 done
