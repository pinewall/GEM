#!/bin/bash

if [[ $# < 3 ]]
then
    echo "Usage: ./formatconvert.sh dir-name old-suffix new-suffix"
    exit 1
fi
rootdir=$1
oldsuffix=$2
newsuffix=$3

find ${rootdir} -name "*.${oldsuffix}" > "formatconvert.log"

while read line
do
    echo ${line}
    name=`basename ${line} .${oldsuffix}`
    path=`dirname ${line}`
    echo ${path}/${name}
    gs -dNOPAUSE -r300 -dBATCH -sOutputFile=${path}/${name}.${newsuffix} -sDEVICE=${newsuffix} ${path}/${name}.${oldsuffix}
done < formatconvert.log
rm "formatconvert.log"
