#!/bin/bash

bin_dir="./bin"
include_dir="./include"
lib_dir="./lib"

sdkpkt="./libevent-2.1.8-stable-kylin-aarch64-sdk.tar.gz"

if [ -d $bin_dir ];
then
rm -rf $bin_dir
else
echo "$bin_dir not exist"
fi

if [ -d $include_dir ];
then
rm -rf $include_dir
else
echo "$include_dir not exist"
fi

if [ -d $lib_dir ];
then
rm -rf $lib_dir
else
echo "$lib_dir not exist"
fi

if [ -f $sdkpkt ];
then
echo "=================begin decompression $sdkpkt====================="
tar -xvf $sdkpkt
echo "=================decompression $sdkpkt end======================="

#删除软连接
echo "--->del libevent_core soft linke"
rm -f ./lib/libevent_core-2.1.so.6
rm -f ./lib/libevent_core.so

cp -f ./lib/libevent_core-2.1.so.6.0.2 ./lib/libevent_core.so
cp -f ./lib/libevent_core-2.1.so.6.0.2 ./lib/libevent_core-2.1.so.6
#删除软连接
echo "--->del libevent_extra soft linke"
rm -f ./lib/libevent_extra-2.1.so.6
rm -f ./lib/libevent_extra.so

cp -f ./lib/libevent_extra-2.1.so.6.0.2 ./lib/libevent_extra.so
cp -f ./lib/libevent_extra-2.1.so.6.0.2 ./lib/libevent_extra-2.1.so.6
#删除软连接
echo "--->del libevent_openssl soft linke"
rm -f ./lib/libevent_openssl-2.1.so.6
rm -f ./lib/libevent_openssl.so

cp -f ./lib/libevent_openssl-2.1.so.6.0.2 ./lib/libevent_openssl.so
cp -f ./lib/libevent_openssl-2.1.so.6.0.2 ./lib/libevent_openssl-2.1.so.6
#删除软连接
echo "--->del libevent_pthreads soft linke"
rm -f ./lib/libevent_pthreads-2.1.so.6
rm -f ./lib/libevent_pthreads.so

cp -f ./lib/libevent_pthreads-2.1.so.6.0.2 ./lib/libevent_pthreads.so
cp -f ./lib/libevent_pthreads-2.1.so.6.0.2 ./lib/libevent_pthreads-2.1.so.6
#删除软连接
echo "--->del libevent soft linke"
rm -f ./lib/libevent-2.1.so.6
rm -f ./lib/libevent.so

cp -f ./lib/libevent-2.1.so.6.0.2 ./lib/libevent.so
cp -f ./lib/libevent-2.1.so.6.0.2 ./lib/libevent-2.1.so.6
else
echo "$sdkpkt not exist"
exit
fi

echo "pre libevent_core libevent_extra libevent_openssl libevent_pthreads libevent ok..." 