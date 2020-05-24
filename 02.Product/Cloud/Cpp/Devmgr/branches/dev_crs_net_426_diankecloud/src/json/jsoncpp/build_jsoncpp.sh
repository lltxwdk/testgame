#!/bin/bash
libname="../../../build/linux/third-party/jsoncpp-0.10.7/lib/64/libjsoncpp.a"


make clean;
make

if [ -f $libname ];
then
echo "$libname ok"
cp -rf ./jsoncpp-0.10.7/include/json/ ../../../build/linux/third-party/jsoncpp-0.10.7/include/
else
echo "build error... "
exit
fi

echo "build ok... $libname" 