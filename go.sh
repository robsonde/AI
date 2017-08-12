#!/bin/sh

while (true); do
./ai -n 500 -g 10 -m 5 > /tmp/bob
cat /tmp/bob | tail -1 | mailx -s "brains" robsonde@asinine.net.nz
done


