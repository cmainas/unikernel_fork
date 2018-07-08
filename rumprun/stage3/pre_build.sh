#!/bin/bash 

RUMPRUN_REPO=/path/to/rumprun

## Check if libunwind is patched 
is_patched=$(grep "\-1LL" ${RUMPRUN_REPO}/src-netbsd/sys/lib/libunwind/AddressSpace.hpp)

if [ -n "$is_patched" ]
then
	patch ${RUMPRUN_REPO}/src-netbsd/sys/lib/libunwind/AddressSpace.hpp < as.patch
fi

## move code and files to the corresponding directories
echo "cp ivshmem.c ${RUMPRUN_REPO}/src-netbsd/sys/dev/pci"
cp ivshmem.c ${RUMPRUN_REPO}/src-netbsd/sys/dev/pci
echo "cp files.pci ${RUMPRUN_REPO}/src-netbsd/sys/dev/pci"
cp files.pci ${RUMPRUN_REPO}/src-netbsd/sys/dev/pci/
echo "cp -r libivshmem/ ${RUMPRUN_REPO}/src-netbsd/sys/rump/dev/lib"
cp -r libivshmem/ ${RUMPRUN_REPO}/src-netbsd/sys/rump/dev/lib/
echo "cp Makefile.rumpdevcomp ${RUMPRUN_REPO}/src-netbsd/sys/rump/dev/"
cp Makefile.rumpdevcomp ${RUMPRUN_REPO}/src-netbsd/sys/rump/dev/

## for the syscall
echo "cp syscalls.master ${RUMPRUN_REPO}/src-netbsd/sys/kern/"
cp syscalls.master ${RUMPRUN_REPO}/src-netbsd/sys/kern/
DIR=${PWD}
cd ${RUMPRUN_REPO}/src-netbsd/sys/kern/
chmod +x makesyscalls.sh
./makesyscalls.sh syscalls.conf syscalls.master
cd $DIR
echo "cp my_pipe.h ${RUMPRUN_REPO}/src-netbsd/sys/kern/"
cp my_pipe.h ${RUMPRUN_REPO}/src-netbsd/sys/kern/
echo "cp sys_my_pipe.c ${RUMPRUN_REPO}/src-netbsd/sys/kern/"
cp sys_my_pipe.c ${RUMPRUN_REPO}/src-netbsd/sys/kern/
echo "cp Makefile.rumpkern ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern/"
cp Makefile.rumpkern ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern/
echo "cp rumpkern_syscalls.c ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern"
cp rumpkern_syscalls.c ${RUMPRUN_REPO}/src-netbsd/sys/rump/librump/rumpkern/

