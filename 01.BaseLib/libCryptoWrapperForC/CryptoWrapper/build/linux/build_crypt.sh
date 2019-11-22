#!/bin/bash
libname="../../lib/libCryptoWrapper.so"

if [ -f $libname ];then
rm -f $libname
echo ">>>>>>>>>>>> rm -f $libname >>>>>>>>>>>>"
fi

echo ">>>>>>>>>>>> Clean Complie Object File >>>>>>>>>>>>"
make clean;
echo "" 

echo ">>>>>>>>>>>> Start Complie .so lib file >>>>>>>>>>>>"
make
echo "" 

if [ -f $libname ];
then
echo "$libname ok"
else
echo "build error... "
exit
fi

echo "build ok... $libname" 