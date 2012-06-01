#!/bin/bash

cd src
make
sudo insmod mobile_host.ko
cd ../tests
./make_tests.sh

