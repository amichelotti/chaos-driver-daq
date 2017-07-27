#!/bin/bash
lista="libera13 libera12 libera10 libera02 libera03 libera05 libera06 libera07 libera08 libera09 libera01"

for i in $lista;do
echo "* connecting to $i"
ssh root@$i /etc/init.d/chaos-us.sh stop
done
sleep 6
for i in $lista;do
echo "* connecting to $i"
ssh root@$i /etc/init.d/chaos-us.sh start
done
