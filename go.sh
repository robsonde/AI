#!/bin/sh

while (true); do
./game -n 1000 -g 1000 -m 1 > /tmp/bob
cat /tmp/bob | tail -1 | mailx -s "brains" robsonde@asinine.net.nz
done


