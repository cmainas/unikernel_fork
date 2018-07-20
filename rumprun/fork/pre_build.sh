#!/bin/bash 

RUMPRUN_REPO=/path/to/rumprun
QEMU_DIR=/path/to/qemu
QEMU_BUILD_DIR=/path/to/qemu_build

qemu=1
rumprun=1
print_usage () {
	echo "Usage: ./pre_build.sh [-qarh] "
	echo -e "\t-a:\t copy files for rumprun and qemu (default option)"
	echo -e "\t-r:\t copy files only for rumprun"
	echo -e "\t-q:\t copy files only for qemu"
	echo -e "\t-h:\t print this help"
}

while getopts ":arqh" opt; do
	case $opt in
		a)
			qemu=1
			rumprun=1
			;;
		q)
			qemu=1
			rumprun=0
			;;
		r)
			qemu=0
			rumprun=1
			;;
		h)
			print_usage
			exit
			;;
		\?)
			echo "Invalid option: -$OPTARG" 
			echo "Use option -h for help" 
			exit
			;;
	esac
done

if [ $qemu -eq 1 ] 
then
	echo 
	echo "----------------- build qemu -----------------"
	echo 
	echo "cp kvm-all.c ${QEMU_DIR}/accel/kvm/kvm-all.c"
	cp kvm-all.c ${QEMU_DIR}/accel/kvm/kvm-all.c
	DIR=${PWD}
	cd ${QEMU_BUILD_DIR}
	make
	cd $DIR
fi

if [ $rumprun -eq 1 ] 
then
	## Check if libunwind is patched 
	is_patched=$(grep "\-1LL" ${RUMPRUN_REPO}/src-netbsd/sys/lib/libunwind/AddressSpace.hpp)
	
	if [ -n "$is_patched" ]
	then
		patch ${RUMPRUN_REPO}/src-netbsd/sys/lib/libunwind/AddressSpace.hpp < as.patch
	fi
	echo 
	echo "------------ copy files to rumprun ------------"
	echo 
	## move code and files to the corresponding directories
	echo "cp syscalls.master ${RUMPRUN_REPO}/src-netbsd/sys/kern/"
	cp syscalls.master ${RUMPRUN_REPO}/src-netbsd/sys/kern/
	DIR=${PWD}
	cd ${RUMPRUN_REPO}/src-netbsd/sys/kern/
	chmod +x makesyscalls.sh
	./makesyscalls.sh syscalls.conf syscalls.master
	cd $DIR
	echo "cp sys_my_fork.c ${RUMPRUN_REPO}/src-netbsd/sys/kern/"
	cp sys_my_fork.c ${RUMPRUN_REPO}/src-netbsd/sys/kern/
	echo "cp Makefile.rumpkern ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern/"
	cp Makefile.rumpkern ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern/
	echo "cp rumpkern_syscalls.c ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern"
	cp rumpkern_syscalls.c ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern/
fi

