#!/bin/bash

cd src
make
sudo insmod mobile_host.ko

mv *.o ../build/
mv *.ko ../build/
mv modules.order ../build/
mv Module.symvers ../build/
mv mobile_host.mod.c ../build/

cd ../tests
./make_tests.sh

