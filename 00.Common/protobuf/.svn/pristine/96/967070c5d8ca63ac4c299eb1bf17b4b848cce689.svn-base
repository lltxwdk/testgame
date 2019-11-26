#!/bin/bash
libname="../../lib/libprotobuf.a"


rm CMakeCache.txt CMakeFiles cmake_install.cmake Makefile -rf

cmake CMakeLists.txt
make clean;
make -j4


if [ -f $libname ];
then
echo "$libname ok"
else
echo "build error... "
exit
fi

echo "build ok... $libname" 
