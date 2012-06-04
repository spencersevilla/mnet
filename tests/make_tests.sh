#!/bin/bash

rm *.out
gcc -o inet_client.out inet_client.c
gcc -o mhost_client.out mhost_client.c
gcc -o inet_server.out inet_server.c
gcc -o mhost_server.out mhost_server.c
gcc -o mhost_send.out mhost_send.c
