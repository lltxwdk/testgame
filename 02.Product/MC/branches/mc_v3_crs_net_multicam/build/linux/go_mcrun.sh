#! /bin/sh

export LD_LIBRARY_PATH=.

chmod a+x mcrun
ulimit -c unlimited
./mcrun
