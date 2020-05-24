#!/bin/bash
#one build, build netlib and mp, then packet to mp.tar.gz

workdir=$(cd $(dirname $0); pwd)

echo "workdir is:$workdir"
#数据库默认编译mysql
dbtype="MYSQL_DB_DEF"
#dbtype="SHENTONG_DB_DEF"

#操作性系统默认编译centos
ostype="centos"
#ostype="kylin"

configfile="devmgr.conf"
devmgrbin="devmgr"
packetname=""
branchname="crs-net-v5.0.0"
suffix_zip=".zip"
suffix_gz=".tar.gz"

#相对build_devmgr.sh脚本的路径
socketbuild="../../../../../../../../../../../../01.BaseLib/netlib/build/linux/"
socketshell="onebuild.sh"
#相对socketbuild的路径
socketlib="../../lib/libnetlib.a"

#相对build_devmgr.sh脚本的路径
hiredisbuild="../../../../../src/redis-3.0/deps/"
hiredisshell="build_hiredis.sh"
#相对hiredisbuild的路径
hiredislib="../../../build/linux/third-party/redis-3.0.5/lib/64/libhiredis.a"
hiredisdll="../../../build/linux/third-party/redis-3.0.5/lib/64/libhiredis.so"
hiredisdll_v="../../../build/linux/third-party/redis-3.0.5/lib/64/libhiredis.so.0.11"

#相对build_devmgr.sh脚本的路径
sredisdllbuild="../../../../../src/sredisdll/"
sredisdllshell="build_sredisdll.sh"
#相对sredisdllbuild的路径
sredisdlllib="../../build/linux/sredisdll/lib/64/libsredisdll.a"

if [ -n "`uname -a | grep kylin`"  ];then
	ostype="kylin"
fi

#相对build_devmgr.sh脚本的路径
if [ $ostype == "centos" ];then
	if [ -n "`uname -a | grep x86_64`" ];then
		libevent_sdk_path="../../../third-party/libevent-2.1.8-stable/linux/64/"
	elif [ -n "`uname -a | grep aarch64`" ];then
		libevent_sdk_path="../../../third-party/libevent-2.1.8-stable/linux/centos-aarch64/"
	fi
elif [ $ostype == "kylin" ];then
	if [ -n "`uname -a | grep x86_64`" ];then
		libevent_sdk_path="../../../third-party/libevent-2.1.8-stable/linux/64/"
	elif [ -n "`uname -a | grep aarch64`" ];then
		libevent_sdk_path="../../../third-party/libevent-2.1.8-stable/linux/kylin-aarch64/"
	fi
fi
#libevent_sdk_path="../../../third-party/libevent-2.1.8-stable/linux/64/"
prelibeventshell="pre_libevent.sh"
#相对libevent_sdk_path的路径
libevent_core_dll="./lib/libevent_core.so"
libevent_core_dll_v="./lib/libevent_core-2.1.so.6"
libevent_extra_dll="./lib/libevent_extra.so"
libevent_extra_dll_v="./lib/libevent_extra-2.1.so.6"
libevent_openssl_dll="./lib/libevent_openssl.so"
libevent_openssl_dll_v="./lib/libevent_openssl-2.1.so.6"
libevent_pthreads_dll="./lib/libevent_pthreads.so"
libevent_pthreads_dll_v="./lib/libevent_pthreads-2.1.so.6"
libevent_dll="./lib/libevent.so"
libevent_dll_v="./lib/libevent-2.1.so.6"

archtype=""
dependlib=""
cryptowrapperlib_name="libCryptoWrapper.so"
release=0
buildmode="debug"
versionname=""
major_v="2"
minor_v="0"
release_v="3"

killall ntpd
ntpdate cn.ntp.org.cn

if [ -n "$1" ];then
	if [ "$1" == "release" ];then
        echo "release mode"
		release=1
		buildmode="release"
    fi
fi

#build libsocket
cd $socketbuild
chmod 777 $socketshell
source $socketshell $1
if [ -f $socketlib ];
then
echo "$socketlib ok"
else
echo "build error... "
exit
fi

#切换到build_devmgr.sh脚本所在目录
#cd -
cd $workdir

#pre libevent,目前只检查libevent_openssl、libevent_pthreads、libevent
cd $libevent_sdk_path
chmod 777 $prelibeventshell
source $prelibeventshell

if [ -f $libevent_openssl_dll ];
then
echo "$libevent_openssl_dll ok"
else
echo "$libevent_openssl_dll not exsit... "
exit
fi

if [ -f $libevent_openssl_dll_v ];
then
echo "$libevent_openssl_dll_v ok"
else
echo "$libevent_openssl_dll_v not exsit... "
exit
fi

if [ -f $libevent_pthreads_dll ];
then
echo "$libevent_pthreads_dll ok"
else
echo "$libevent_pthreads_dll not exsit... "
exit
fi

if [ -f $libevent_pthreads_dll_v ];
then
echo "$libevent_pthreads_dll_v ok"
else
echo "$libevent_pthreads_dll_v not exsit... "
exit
fi

if [ -f $libevent_dll ];
then
echo "$libevent_dll ok"
else
echo "$libevent_dll not exsit... "
exit
fi

if [ -f $libevent_dll_v ];
then
echo "$libevent_dll_v ok"
else
echo "$libevent_dll_v not exsit... "
exit
fi

#切换到build_devmgr.sh脚本所在目录
#cd -
cd $workdir

#build libhiredis.a
cd $hiredisbuild
chmod 777 $hiredisshell
source $hiredisshell
if [ -f $hiredislib ];
then
echo "$hiredislib ok"
else
echo "$hiredislib not exsit... "
exit
fi

if [ -f $hiredisdll ];
then
echo "$hiredisdll ok"
else
echo "$hiredisdll not exsit... "
exit
fi

if [ -f $hiredisdll_v ];
then
echo "$hiredisdll_v ok"
else
echo "$hiredisdll_v not exsit... "
exit
fi

#切换到build_devmgr.sh脚本所在目录
#cd -
cd $workdir

#check libhiredis.a
dependlib="../../../third-party/redis-3.0.5/lib/64/libhiredis.a"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
else
echo "$dependlib not exsit "
exit
fi

#check libhiredis.so
dependlib="../../../third-party/redis-3.0.5/lib/64/libhiredis.so"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#check libhiredis.so.0.11
dependlib="../../../third-party/redis-3.0.5/lib/64/libhiredis.so.0.11"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#check libevent.so
dependlib="$libevent_sdk_path/lib/libevent.so"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#check libevent-2.1.so.6
dependlib="$libevent_sdk_path/lib/libevent-2.1.so.6"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#check libevent_openssl.so
dependlib="$libevent_sdk_path/lib/libevent_openssl.so"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#check libevent_openssl-2.1.so.6
dependlib="$libevent_sdk_path/lib/libevent_openssl-2.1.so.6"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#check libevent_pthreads.so
dependlib="$libevent_sdk_path/lib/libevent_pthreads.so"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#check libevent_pthreads-2.1.so.6
dependlib="$libevent_sdk_path/lib/libevent_pthreads-2.1.so.6"
if [ -f $dependlib ];
then
echo "$dependlib exsit"
cp -f $dependlib ./
else
echo "$dependlib not exsit "
exit
fi

#切换到build_devmgr.sh脚本所在目录
#cd -
cd $workdir

#build libsredisdll.a
cd $sredisdllbuild
chmod 777 $sredisdllshell
source $sredisdllshell
if [ -f $sredisdlllib ];
then
echo "$sredisdlllib ok"
else
echo "build error... "
exit
fi

#切换到build_devmgr.sh脚本所在目录
#cd -
cd $workdir

if [ $ostype == "kylin" ];then
	if [ -n "`uname -a | grep aarch64`" ];then
		archtype="kylin-aarch64"
		if [ $dbtype == "SHENTONG_DB_DEF" ];then
			cp -f ../../../third-party/shentong/arm64/libaci.so ./
			cp -f ../../../third-party/shentong/arm64/libacci.so ./
		fi
	elif [ -n "`uname -a | grep x86_64`" ];then
		archtype="kylin-x86_64"
		if [ $dbtype == "SHENTONG_DB_DEF" ];then
			cp -f ../../../third-party/shentong/linux64/libaci.so ./
			cp -f ../../../third-party/shentong/linux64/libacci.so ./
		fi
	fi
elif [ $ostype == "centos" ];then
	if [ -n "`uname -a | grep aarch64`" ];then
		archtype="centos-aarch64"
		if [ $dbtype == "SHENTONG_DB_DEF" ];then
			cp -f ../../../third-party/shentong/arm64/libaci.so ./
			cp -f ../../../third-party/shentong/arm64/libacci.so ./
		fi
	elif [ -n "`uname -a | grep x86_64`" ];then
		archtype="centos-x86_64"
		if [ $dbtype == "SHENTONG_DB_DEF" ];then
			cp -f ../../../third-party/shentong/linux64/libaci.so ./
			cp -f ../../../third-party/shentong/linux64/libacci.so ./
		fi
	fi
fi

if [ -f "Makefile" ];then
echo "rm Makefile"
rm Makefile
fi

if [ -f "cmake_install.cmake" ];then
echo "rm cmake_install.cmake"
rm cmake_install.cmake
fi

if [ -f "CMakeCache.txt" ];then
echo "rm CMakeCache.txt"
rm CMakeCache.txt
fi

if [ -d "CMakeFiles" ];then
echo "rm CMakeFiles/ -rf"
rm CMakeFiles/ -rf
fi

cmake -DARCH=$archtype -DDBTYPE=$dbtype CMakeLists.txt
make clean;make

#check devmgr
if [ -f $devmgrbin ];
then
echo "$devmgrbin ok"
else
echo "$devmgrbin build error... "
exit
fi

#check devmgr.conf
if [ -f $configfile ];
then
echo "$configfile exsit"
else
echo "$configfile not exsit, please copy it"
exit
fi

#check hiredis dll file --> libhiredis.so libhiredis.so.0.11
if [ -f "libhiredis.so" ];then
	echo "libhiredis.so exsit"
else
	echo "libhiredis.so not exsit, please copy it"
	exit
fi
if [ -f "libhiredis.so.0.11" ];then
	echo "libhiredis.so.0.11 exsit"
else
	echo "libhiredis.so.0.11 not exsit, please copy it"
	exit
fi

#check shentong dll file --> libaci.so libacci.so
if [ $dbtype == "SHENTONG_DB_DEF" ];then
	if [ -f "libaci.so" ];then
		echo "libaci.so exsit"
	else
		echo "libaci.so not exsit, please copy it"
		exit
	fi
	if [ -f "libacci.so" ];then
		echo "libacci.so exsit"
	else
		echo "libacci.so not exsit, please copy it"
		exit
	fi
fi

echo "begin target packet...."

#PKGDATE=$(date +%Y%m%d-%H%M)
if [ $dbtype == "SHENTONG_DB_DEF" ];then
	packetname=$devmgrbin-$branchname-$archtype-shentongdb$suffix_gz
else
	packetname=$devmgrbin-$branchname-$archtype$suffix_gz
fi

if [ -f $packetname ];then
echo "rm -f old packetname is:$packetname"
rm -f $packetname
fi

VERDATE=$(date +%Y%m%d%H%M)
versionname=$major_v.$minor_v.$release_v.$VERDATE

echo -e "version:$versionname\nbuild mode:$buildmode" > version

echo "new packetname is:$packetname"

if [ $dbtype == "SHENTONG_DB_DEF" ];then
	tar czvf $packetname $devmgrbin $configfile devmgr.sh libhiredis.so libhiredis.so.0.11 libaci.so libacci.so libevent.so libevent-2.1.so.6 libevent_openssl.so libevent_openssl-2.1.so.6 libevent_pthreads.so libevent_pthreads-2.1.so.6 version
else
	tar czvf $packetname $devmgrbin $configfile devmgr.sh libhiredis.so libhiredis.so.0.11 libevent.so libevent-2.1.so.6 libevent_openssl.so libevent_openssl-2.1.so.6 libevent_pthreads.so libevent_pthreads-2.1.so.6 version
fi

echo "build ok... " 
