#!/bin/bash
lista="libera01 libera02 libera03 libera07 libera08 libera09"
for i in $lista;do
echo "* connecting to $i"
ssh root@$i /etc/init.d/libera-cu.sh restart
done
