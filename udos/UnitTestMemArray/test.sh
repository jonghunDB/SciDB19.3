#!/bin/bash

MYDIR=`dirname $0`
pushd $MYDIR > /dev/null
MYDIR=`pwd`
OUTFILE=$MYDIR/test.out
EXPFILE=$MYDIR/test.expected


echo "[">> date >> "]"
date  >> $OUTFILE 2>&1

time iquery -aq "test_memarray(0.01)"



