#!/bin/bash 

RUMPRUN_REPO=/path/to/rumprun_repo

## Check if libunwind is patched 
is_patched=$(grep "\-1LL" ${RUMPRUN_REPO}src-netbsd/sys/lib/libunwind/AddressSpace.hpp)

if [ -n "$is_patched" ]
then
	patch ${RUMPRUN_REPO}/src-netbsd/sys/lib/libunwind/AddressSpace.hpp < as.patch
fi

## move code and files to the corresponding directories
echo "cp dev_comso.c ${RUMPRUN_REPO}/src-netbsd/sys/dev/"
cp dev_comso.c ${RUMPRUN_REPO}/src-netbsd/sys/dev/
echo "cp dev_comso.h ${RUMPRUN_REPO}/src-netbsd/sys/dev/"
cp dev_comso.h ${RUMPRUN_REPO}/src-netbsd/sys/dev/
echo "cp majors ${RUMPRUN_REPO}/src-netbsd/sys/conf"
cp majors ${RUMPRUN_REPO}/src-netbsd/sys/conf/
echo "cp files ${RUMPRUN_REPO}/src-netbsd/sys/conf"
cp files ${RUMPRUN_REPO}/src-netbsd/sys/conf/
echo "cp -r libcomso/ ${RUMPRUN_REPO}/src-netbsd/sys/conf"
cp -r libcomso/ ${RUMPRUN_REPO}/src-netbsd/sys/rump/dev/lib/
echo "cp Makefile.rumpdevcomp ${RUMPRUN_REPO}/src-netbsd/sys/rump/dev/"
cp Makefile.rumpdevcomp ${RUMPRUN_REPO}/src-netbsd/sys/rump/dev/
