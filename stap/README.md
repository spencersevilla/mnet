Intro and Installation
======================

stap is a Linux utility for monitoring kernel activity/functions, similar to dtrace. These scripts are designed to give a much clearer understanding of what's going on inside the kernel network stack when certain functions are called. To run these scripts:

0. First, you must have the debug symbols installed for your kernel version. Get these symbols by running the script ./get-dbgsym.

1. Next, make sure you have systemtap installed. On Ubuntu, this looks like "sudo apt-get install systemtap".

1.5 Depending on your hardware, software, OS, and the favor of the gods, systemtap either will or will not work. You may have to recompile it or google cryptic error messages. A good resource for Ubuntu can be found here: http://www.domaigne.com/blog/random/running-systemtap-on-ubuntu/.
Best way to test is by running the super-simple command below:
$ sudo stap -e 'probe kernel.function("sys_open") {log("hello world") exit()}'

2. Once systemtap is working, you can run any of the scripts included in this folder simply by executing the command $ sudo stap <filename>. The program will run continuously in that terminal and print out lines as appropriate.


The Scripts
============

In a nutshell, these are very specific scripts that correspond with the programs in /tests/ (which is why the names line up). For a given program, these scripts will trace certain kernel functions called as a result of the program (the arrows to the left of the function call indicate if the function is entering or returning). The kernel functions traced are all the functions that appear in one of the files below:
net/ipv4/udp.c
net/ipv4/ip_input.c
net/core/dev.c
net/ipv4/route.c