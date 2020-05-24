#!/bin/bash

CMD="pgrep -x"
export LD_LIBRARY_PATH=.

ulimit -c unlimited

check_proc_alive()
{
	local alive_pids="`$CMD $PPROC`"
	local alive_pid=0
#	echo "check_proc_alive func $PPROC pids: $alive_pids"
	for alive_pid in ${alive_pids};
	do
#		ALIVE_PID_CWDPATH="`ls -l /proc/$alive_pid/cwd | awk '{print$11}'`"
		ALIVE_PID_EXEPATH="`ls -l /proc/$alive_pid/exe | awk '{print$11}'`"
#		ALIVE_PID_PWDPATH="`pwd`"
#		echo $ALIVE_PID_CWDPATH
#		echo $ALIVE_PID_EXEPATH
#		echo $ALIVE_PID_PWDPATH
		if [ "$ALIVE_PID_EXEPATH" = "$exe_full_path_name" ];then
#			echo "$PPROC $alive_pid in $ALIVE_PID_CWDPATH is running..."
#			return $alive_pid;
			echo $alive_pid
			exit
		fi
	done
	
#	return 0;
	echo 0;
}

# 前提是用devmgr.sh启动：
# /root/....../xxx.sh start
check_start_shell_pid()
{
	local l_start_shell_pids="`pgrep -x $shell_name`"
	local l_start_shell_pid=0
#	echo " in check_start_shell_pid func $shell_name pids: $l_start_shell_pids"

	#获取进程pid（即传入的参数$1）对应的ppid
#	local l_start_proc_ppids="`ps -ef | grep $PPROC | grep -v grep | awk '{print$3}'`"
#	local l_start_proc_ppid=-1

#	for l_start_shell_pid in ${l_start_shell_pids};
#	do
#		for l_start_proc_ppid in ${l_start_proc_ppids};
#		do
#			if [ "$l_start_proc_ppid" = "$l_start_shell_pid" ];then
#				echo "$PPROC $l_start_shell_pid in $START_SHELL_PID_CWDPATH is running..."
#				echo $l_start_shell_pid
#				exit
#			fi
#		done
#	done

	#获取进程pid（即传入的参数$1）对应的ppid
	local l_start_proc_ppid="`cat /proc/$1/stat | awk '{print$4}'`"
#	echo "check_start_shell_pid func $1 ppid: $l_start_proc_ppid"
	
	if [ $l_start_proc_ppid -ne 0 ];then
		for l_start_shell_pid in ${l_start_shell_pids};
		do
			if [ "$l_start_proc_ppid" = "$l_start_shell_pid" ];then
#				echo "$PPROC $l_start_shell_pid in $START_SHELL_PID_CWDPATH is running..."
				echo $l_start_shell_pid
				exit
			fi
		done
	fi
	echo 0;
}

auto_check_alive()
{
while :
do
	auto_proc_pid=`check_proc_alive`
	echo "auto_proc_pid is: $auto_proc_pid"
	if [ $auto_proc_pid -eq 0 ];then
		echo "$PPROC is killed auto restarting..."
		chmod +x $exe_full_path_name
		$exe_full_path_name -daemon
		
	else
		echo " auto_check_alive func $PPROC $auto_proc_pid is already running..."
	fi

	sleep 5
done

}

#可能是全路径（绝对路径），可能是相对路径
# /root/....../xxx.sh
# ./xxx.sh
shell_path_name=${0}
echo "shell_path_name:"$shell_path_name

#本shell脚本的绝对路径，不包括/xxx.sh
shell_absolute_path=$(cd "$(dirname "$0")"; pwd)
echo "shell_absolute_path:"$shell_absolute_path

# %/* 表示从右边开始，删除第一个 / 号及右边的字符
#shell_cur_path=${shell_path_name%/*}
#echo "shell_cur_path:"$shell_cur_path

# ##*/ 表示从左边开始删除最后（最右边）一个 / 号及左边的所有字符
shell_name=${shell_path_name##*/} 
echo "shell_name:"$shell_name

# %.* 表示从右边开始，删除第一个 . 号及右边的字符
PPROC=${shell_name%.*}

#进程对应的全路径
exe_full_path_name="$shell_absolute_path""/""$PPROC"
echo "exe_full_path_name:"$exe_full_path_name

#shell脚本对应的全路径
shell_full_path_name="$shell_absolute_path""/""$shell_name"
echo "shell_full_path_name:"$shell_full_path_name


echo ${1}ing...

if [ -z "$1" ];then
	echo "parm is null usage $shell_name [start] [stop]" 
	exit
fi

if [ "$1" = "start" ];then
	echo "will start $PPROC"
	start_proc_pid=`check_proc_alive`
	echo "start_proc_pid is:" $start_proc_pid
	if [ $start_proc_pid -eq 0 ];then
		echo "into start path: $exe_full_path_name"
#		cd $shell_absolute_path
		echo "$PPROC is starting..."
		chmod +x $exe_full_path_name
		$exe_full_path_name -daemon
#		chmod +x $PPROC
#		./$PPROC -daemon
		
		auto_check_alive
	else
		echo "$PPROC $start_proc_pid already running..."
	fi
elif [ "$1" = "stop" ];then
	stop_proc_pid=`check_proc_alive`
	if [ $stop_proc_pid -ne 0 ];then
#		echo "into stop path: $shell_absolute_path"
#		cd $shell_absolute_path
#		echo $pwd/$PPROC.sh
		start_shell_pid=`check_start_shell_pid $stop_proc_pid`
		echo " stop_proc_pid -> start_shell_pid: $stop_proc_pid -> $start_shell_pid"
		if [ $start_shell_pid -ne 0 ];then
			kill -9 $start_shell_pid
			echo "$shell_name pid: $start_shell_pid is killed"
		fi
		
		kill -9 $stop_proc_pid
		echo "$PPROC pid: $stop_proc_pid is killed"
	else
		echo "stop $PPROC not running..."
	fi
	
	exit
else
	echo "usage $shell_name [start] [stop] "
fi

