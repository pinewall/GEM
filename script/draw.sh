#!/bin/bash

# prepare original netCDF file "test.nc"
ncfile="../test.nc"

# check existence of "unit_test"

# run "unit_test" and generate log file "value"
logfile="value"

# seperate log file into four files
#awk "{print $1}" $logfile > af
#awk "{print $2}" $logfile > o1
#awk "{print $3}" $logfile > o2a
#awk "{print $4}" $logfile > o2d

# run ncl script
ncl compare.ncl
