#!/bin/bash
#one build, build netlib and mp, then packet to mp.tar.gz

configfile="mc.conf"
mcbin="mcrun"
packetname=""
branchname="crs-net-multicam"
suffix_zip=".zip"
suffix_gz=".tar.gz"
#����ϵͳĬ�ϱ���centos
ostype="centos"
#ostype="kylin"

if [ -n "`uname -a | grep kylin`"  ];then
	ostype="kylin"
fi

socketbuild="../../../../../../01.BaseLib/netlib/build/linux/"
socketshell="onebuild.sh"
socketlib="../../lib/libnetlib.a"
cryptowrapperbuild="../../../../../../01.BaseLib/libCryptoWrapperForC/CryptoWrapper/build/linux/"
cryptshell="build_crypt.sh"
cryptowrapperlib="../../lib/libCryptoWrapper.so"
archtype=""
dependlib=""
cryptowrapperlib_name="libCryptoWrapper.so"
release=0
buildmode="debug"
versionname=""
major_v="1"
minor_v="7"
release_v="24"

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

#�л���build_mc.sh�ű�����Ŀ¼
cd -

#build libCryptoWrapper
cd $cryptowrapperbuild
chmod 777 $cryptshell
source $cryptshell
if [ -f $cryptowrapperlib ];
then
echo "$cryptowrapperlib ok"
else
echo "build error... "
exit
fi

#�л���build_mc.sh�ű�����Ŀ¼
cd -

dependlib="../../../../../../01.BaseLib/libCryptoWrapperForC/CryptoWrapper/lib/libCryptoWrapper.so"
#check libCryptoWrapper
if [ -f $dependlib ];
then
echo "$dependlib ok"
else
echo "$dependlib not exsit "
exit
fi

cp -f $dependlib ./

if [ $ostype == "kylin" ];then
	if [ -n "`uname -a | grep aarch64`" ];then
		archtype="kylin-aarch64"
		cp -f $dependlib ../../src/crypt_4_0_sdk/kylin/aarch64
	elif [ -n "`uname -a | grep x86_64`" ];then
		archtype="kylin-x86_64"
		cp -f $dependlib ../../src/crypt_4_0_sdk/kylin/x86_64
	fi
	cp -f ../../../../../../01.BaseLib/libCryptoWrapperForC/CryptoWrapper/interface/CryptoWrapperAPI.h ../../src/crypt_4_0_sdk/kylin
elif [ $ostype == "centos" ];then
	if [ -n "`uname -a | grep aarch64`" ];then
		archtype="centos-aarch64"
		cp -f $dependlib ../../src/crypt_4_0_sdk/centos/aarch64
	elif [ -n "`uname -a | grep x86_64`" ];then
		archtype="centos-x86_64"
		cp -f $dependlib ../../src/crypt_4_0_sdk/centos/x86_64
	fi
	cp -f ../../../../../../01.BaseLib/libCryptoWrapperForC/CryptoWrapper/interface/CryptoWrapperAPI.h ../../src/crypt_4_0_sdk/centos
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

cmake -DARCH=$archtype CMakeLists.txt
make clean;make

#check error
if [ -f $mcbin ];
then
echo "$mcbin ok"
else
echo "build error... "
exit
fi

#check files
if [ -f $configfile ];
then
echo "$configfile exsit"
else
echo "$configfile not exsit, please copy it"
exit
fi

echo "begin target packet...."

#PKGDATE=$(date +%Y%m%d%-H%M)
VERDATE=$(date +%Y%m%d%H%M)
packetname=$mcbin-$branchname-$archtype
versionname=$major_v.$minor_v.$release_v.$VERDATE

echo -e "version:$versionname\nbuild mode:$buildmode" > version

echo "packetname is:$packetname$suffix_gz"

tar czvf $packetname$suffix_gz $mcbin $configfile $cryptowrapperlib_name McLicence.ini mcrun.sh version

echo "build ok... " 