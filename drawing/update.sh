#!/bin/sh
#scp "value" "test.nc" "remap_compare.ncl"  songshunqiang@thucpl2.3322.org:~/ncl/
if [ $# != 1 ]; 
then
    echo "Usage: ./update.sh file"
    exit 1;
fi
scp $1 songshunqiang@thucpl2.3322.org:~/ncl/
