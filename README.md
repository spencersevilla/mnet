mnet
====

LKM for supporting multiple network protocols simultaneously
Currently developed for Linux 3.0.0


Installation and Initialization
====================================

./install.sh should compile and install the LKM mobile_host, in particular the function mhost_init in module_hooks.c.
This function registers:
        - struct proto udpmhost_prot
        - struct net_proto_family mhost_family_ops
        - struct packet_type mhost_ptype
        
...as well as our own, internal table type functions.


Debugging Tools
====================================

Printk()
--------

So, kernel debugging is a royal pain, there's no getting around it. The best recommendation I can make is to put printk()'s everywhere to figure out what's happening. You can see these printk()'s in /var/log/kern.log or by using dmesg, though personally I prefer tail -f on the logfile itself. This logfile persists across reboots and will dump a stacktrace if an error occurs in the code of the module itself. Thus, if your machine panics, a reboot followed by "tail /var/log/kern.log" will point you in the right direction.

SystemTap
----------

Your other big debugging tool is system tap, a language designed to probe kernel functions without requiring kernel recompilation itself. Essentially, it is the Linux flavor of dtrace or ftrace, if you've used either of these utilities before. I have included some stap scripts (in /stap/) along with a more detailed readme file on how to use them.


Testing
========

Sample C programs can be found in /tests/. make_tests.sh should compile all the programs fine. inet_client.c uses regular AF_INET sockets to send a string to localhost:8080, inet_server.c listens on this port and prints out strings received. mhost_client.c and mhost_server.c do the exact same thing, but using AF_MHOST sockets.


Packet Flow
====================================

App To MNET
-----------

After the module is installed, any one of the test programs may be run.

When an AF_MNET socket is created, it uses the mhost_family_ops function to call mhost_create and AUTOMATICALLY sets the protocol to SOCK_STREAM (UDP)

When send*() is called by an application on an AF_MNET socket, it follows the normal Linux Kernel packet-flow up until sock->ops->sendmsg is called (typically demuxes to inet_sendmsg). Here, mhost_sendmsg() is called instead and we start using our own functions

mhost_sendmsg() is generally an error-checking wrapper function (binds if necessary), but MOST IMPORTANTLY it calls mhost_translate_sa to set the proper L3 handler!

mhost_translate_sa() works by demuxing sa->sa_family to see what the application has passed. If AF_UNSPEC then we bail out, if specified (ie AF_INET) then we set mhost_sock ms->proto and leave the address unchanged, if AF_MHOST then we know that the address given is actually a MHOST pointer, so we must 1) translate the address and 2) set ms->proto appropriately.

*** NOTE: I don't think I have found a way for AF_MNET sockets to bind to an address and RECEIVE IP packets. Maybe? Hmph.


MNET To Wire
------------

*** In the cases of INET and INET6, ms->proto->udpsendmsg demuxes to udp_sendmsg

udpmhost_sendmsg() starts by checking for ms->proto->udpsendmsg: if it provides a udp handler, just pass it along and wipe our hands of the whole damn thing! Note: this IS the case for all AF_INET and AF_INET6 addresses. If not, that means that whatever protocol we're using has elected to use OUR udp function instead, so we go through UDP functionality (explicitly NOT setting a cksum, since I couldn't figure out how :-p) and eventually we call ms->proto->sendmsg which delivers it to the appropriate L3 handler...