#!/bin/bash
libname="./libhiredis.a"
dllname="./libhiredis.so"
dllname_v="./libhiredis.so.0.11"

dst_libname="../../../../build/linux/third-party/redis-3.0.5/lib/64/libhiredis.a"
dst_dllname="../../../../build/linux/third-party/redis-3.0.5/lib/64/libhiredis.so"
dst_dllname_v="../../../../build/linux/third-party/redis-3.0.5/lib/64/libhiredis.so.0.11"

tar -xvf hiredis-0.11.0.tar.gz

cd ./hiredis-0.11.0

make clean;
make

if [ -f $libname ];
then
echo "$libname build ok"
cp -f $libname $dst_libname
else
echo "$libname build error... "
exit
fi

if [ -f $dllname ];
then
echo "$dllname build ok"
cp -f $dllname $dllname_v
cp -f $dllname $dst_dllname
else
echo "$dllname build error... "
exit
fi

if [ -f $dllname_v ];
then
echo "$dllname_v build ok"
cp -f $dllname_v $dst_dllname_v
else
echo "$dllname_v build error... "
exit
fi

cd ../
