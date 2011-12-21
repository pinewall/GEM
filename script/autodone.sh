#!/bin/bash

# lists of grid pairs
grids="G128_T42"
#grids="deg2.5_deg1 G128_T42 T42_POP43"

# lists of test cases
cases="coslat_coslon cosbell Y2_2 Y16_32"

# root directory for output
rootdir="version4"

if [ ! -d ${rootdir} ]
then
    mkdir ${rootdir}
fi

# executable filename
exepath="."
exefile="unit_test"
nclpath="."
nclnormal="remap_compare.ncl"
nclnative="remap_compare2.ncl"

# check existence of grid pairs
for g in ${grids}
do
    if [ ! -f "${g}.nc" ]
    then
        echo "File ${g}.nc does not exist in current path"
        exit 1
    fi
done

# loop for all grid pairs
for g in ${grids}
do
    echo "${g}...Init"
    # make directory for current grid pair if necessory
    if [ ! -d ${rootdir}/${g} ]
    then
        mkdir ${rootdir}/${g}
    fi
    # loop for all test cases
    for c in ${cases}
    do
        # make directory for current case
        if [ ! -d ${rootdir}/${g}/${c} ]
        then
            mkdir ${rootdir}/${g}/${c}
        fi
        # run unit_test for current grid pair and case
        ${exepath}/${exefile} "${g}.nc" ${c} > "${g}.${c}.value"
        cp "${g}.${c}.value" "value"

        # run ncl script to draw
#if [ ! -s "value" ]
#        then
#            echo "File value should not be empty"
#        fi
        if [ -f "value" ]
        then
            ncl ${nclnormal}
            # mv files to corresponding directory
            mv ${g}.${c}.* ${rootdir}/${g}/${c}/
        fi
    done
    echo "${g}...Done"
done
