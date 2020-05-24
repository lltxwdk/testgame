#! /bin/sh

export LD_LIBRARY_PATH=.

chmod a+x devmgr
ulimit -c unlimited
./devmgr 
