obj-m += mobile_host.o
mobile_host-objs := module_hooks.o af_mhost.o udp_mhost.o mhost_table.o mhost_inet.o mhost_upstack.o mhost_downstack.o mhost_testproto.o udp_table.o  

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD/src) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD/src) clean 
	rm -f *.ko *.o .*.tmp .*.cmd
