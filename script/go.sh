#!/bin/bash

if [[ $# < 3 ]]
then
    echo "Usage: ./test src_grid_name dst_grid_name field_name"
    exit 1
else
    SRCGRIDNAME=$1
    DSTGRIDNAME=$2
    FIELDNAME=$3
    #if [[ $FIELDNAME == "Y2_2" ]]
    #then
    #    ./unit_test maps/${SRCGRIDNAME}_${DSTGRIDNAME}.nc xml/${SRCGRIDNAME}_${DSTGRIDNAME}.xml "Y2_2" > ${SRCGRIDNAME}_${DSTGRIDNAME}.ulog
    #elif [[ $FIELDNAME == "lwdn" ]]
    echo "./unit_test maps/${SRCGRIDNAME}_${DSTGRIDNAME}.nc xml/${SRCGRIDNAME}_${DSTGRIDNAME}.xml realdata ../real_data/spline_none/fields/${SRCGRIDNAME}.${FIELDNAME}.nc ../real_data/spline_none/fields/${SRCGRIDNAME}.${FIELDNAME}.xml ../real_data/spline_none/fields/${DSTGRIDNAME}.${FIELDNAME}.nc ../real_data/spline_none/fields/${DSTGRIDNAME}.${FIELDNAME}.xml"
    #./unit_test "maps/${SRCGRIDNAME}_${DSTGRIDNAME}.nc" "xml/${SRCGRIDNAME}_${DSTGRIDNAME}.xml" "realdata" "../real_data/spline_none/fields/${SRCGRIDNAME}.${FIELDNAME}.nc" "../real_data/spline_none/fields/${SRCGRIDNAME}.${FIELDNAME}.xml" "../real_data/spline_none/fields/${DSTGRIDNAME}.${FIELDNAME}.nc" "../real_data/spline_none/fields/${DSTGRIDNAME}.${FIELDNAME}.xml"
    #then
    #fi
fi
