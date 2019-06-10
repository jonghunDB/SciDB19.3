#!/bin/bash

MYDIR=`dirname $0`
pushd $MYDIR > /dev/null
MYDIR=`pwd`
OUTFILE=$MYDIR/test.out
EXPFILE=$MYDIR/test.expected

echo "cp lib file"
sudo cp libUnitTestMemArray.so /opt/scidb/19.3/lib/scidb/plugins

echo "scidb stop"
scidbctl.py stop

echo "sleep 10s "
sleep 10

echo "scidb start"
scidbctl.py start

echo " " >> $OUTFILE 2>&1
echo "Chapter 1" >> $OUTFILE 2>&1

time iquery -aq "load_library('UnitTestMemArray')"

time iquery -aq "test_memarray()"



