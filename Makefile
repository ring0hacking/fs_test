obj-m := fs_test.o

all:
	make -C /usr/src/linux-headers-`uname -r` M=`pwd` modules && insmod fs_test.ko && rmmod fs_test
clean:
	make -C /usr/src/linux-headers-`uname -r` M=`pwd` clean
