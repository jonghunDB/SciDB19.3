#!/bin/bash

MYDIR=`dirname $0`
pushd $MYDIR > /dev/null
MYDIR=`pwd`
OUTFILE=$MYDIR/test.out
EXPFILE=$MYDIR/test.expected

beginTime=$(date +%s%N)

#################################################### density =1 #########################################################################

# 10 times Iterator
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"
time iquery -aq "test_memarray(0,500000,1,10000,0,0)"

# 10 times map
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"
time iquery -aq "test_memarray(0,500000,1,10000,0,1)"


# 10 times Iterator
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,0)"

# 10 times map
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,0,1)"

# 10 times Iterator
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,0)"

# 10 times map
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,0,1)"


#################################################### density = 0.5 #########################################################################

# 10 times Iterator
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"
time iquery -aq "test_memarray(0,500000,1,10000,50,0)"

# 10 times map
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"
time iquery -aq "test_memarray(0,500000,1,10000,50,1)"


# 10 times Iterator
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,0)"

# 10 times map
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,50,1)"

# 10 times Iterator
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,0)"

# 10 times map
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,50,1)"


#################################################### density = 0.33 #########################################################################

# 10 times Iterator
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"
time iquery -aq "test_memarray(0,500000,1,10000,66,0)"

# 10 times map
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"
time iquery -aq "test_memarray(0,500000,1,10000,66,1)"


# 10 times Iterator
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,0)"

# 10 times map
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,66,1)"

# 10 times Iterator
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,0)"

# 10 times map
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,66,1)"


############################################################# density =0.1 ######################################################################################
# 10 times Iterator
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"
time iquery -aq "test_memarray(0,500000,1,10000,90,0)"

# 10 times map
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"
time iquery -aq "test_memarray(0,500000,1,10000,90,1)"


# 10 times Iterator
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,0)"

# 10 times map
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,90,1)"

# 10 times Iterator
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,0)"

# 10 times map
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,90,1)"




############################################################# density =0.01 ######################################################################################
# 10 times Iterator
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"
time iquery -aq "test_memarray(0,500000,1,10000,99,0)"

# 10 times map
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"
time iquery -aq "test_memarray(0,500000,1,10000,99,1)"


# 10 times Iterator
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,0)"

# 10 times map
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,5000000,1,100000,99,1)"

# 10 times Iterator
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,0)"

# 10 times map
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"
time iquery -aq "test_memarray(0,10000000,1,100000,99,1)"



###################################################################################################################################################
endTime=$(date +%s%N)
elapsed=`echo "($endTime - $beginTime) / 1000000" | bc`
elapsedSec=`echo "scale=6;$elapsed / 1000" | bc | awk '{printf "%.6f", $1}'`

echo TOTAL: $elapsedSec sec >> $OUTFILE 2>&1

#echo "[" >> date >> "]"
#echo "Test Iterator "
#date  >> $OUTFILE 2>&1





#echo "[">> date >> "]"
#echo "Test Map "
#date  >> $OUTFILE 2>&1
#
#time iquery -aq "test_memarray(0,50000000,1,1000000,0,1)"