#!/bin/bash
libname="../../build/linux/sredisdll/lib/64/libsredisdll.a"


make clean;
make

if [ -f $libname ];
then
echo "$libname ok"
cp -f ./sredisdll.h ../../build/linux/sredisdll/include/
else
echo "build error... "
exit
fi

echo "build ok... $libname" 