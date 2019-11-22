#!/bin/bash
libname="../../lib/libnetlib.a"

release=1

if [ -n "$1" ];then
	    if [ "$1" == "release" ];then
			echo "release mode"
			release=1
    fi
fi

echo "build ... "
echo "$release"


cmake -DRELEASE=$release CMakeLists.txt
make clean;
make

if [ -f $libname ];
then
echo "$libname ok"
else
echo "build error... "
exit
fi

echo "build ok... $libname" 
