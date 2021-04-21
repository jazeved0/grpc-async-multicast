#!/bin/bash

trap "killall background" EXIT

./bin/server 50543 &
./bin/server 50544 &
./bin/server 50545 &
./bin/server 50546 &
sleep 1

./bin/client 50543 50544 50545 50546
