#!/bin/sh

while (true); do
./ai -n 400 -g 100 -m 2 > /tmp/bob
cat /tmp/bob | tail -1 | mailx -s "brains" robsonde@asinine.net.nz
done


