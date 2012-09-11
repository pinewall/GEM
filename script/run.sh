#!/bin/bash

#./unit_test ll2.5_ll0.5.nc ll2.5_ll0.5.xml Y2_2 > ll2.5_ll0.5.Y2_2
#./unit_test ll2.5_ll0.5.nc ll2.5_ll0.5.xml aaY2_2 > ll2.5_ll0.5.aaY2_2
#time ./gem maps/ll2.5_ll0.5.nc maps/ll2.5_ll0.5.order2.nc maps/ll2.5_ll0.5.xml &> ll2.5_ll0.5.order2.log
#time ./gem maps/ll0.5_ll2.5.nc maps/ll0.5_ll2.5.order2.nc maps/ll0.5_ll2.5.xml &> ll0.5_ll2.5.order2.log
#time ./gem maps/ll2.5_ll1.nc maps/ll2.5_ll1.order2.nc maps/ll2.5_ll1.xml &> ll2.5_ll1.order2.log
#time ./gem maps/ll1_ll2.5.nc maps/ll1_ll2.5.order2.nc maps/ll1_ll2.5.xml &> ll1_ll2.5.order2.log
#time ./gem maps/ll0.5_ll1.nc maps/ll0.5_ll1.order2.nc maps/ll0.5_ll1.xml &> ll0.5_ll1.order2.log
#time ./gem maps/ll1_ll0.5.nc maps/ll1_ll0.5.order2.nc maps/ll1_ll0.5.xml &> ll1_ll0.5.order2.log

if [[ $# < 2 ]]
then
	echo "Usage: ./run.sh grid_name1 grid_name2 [mode:test/time]"
	exit 1
else
	SRCNAME=$1
	DSTNAME=$2
	WORKDIR="/home/songshunqiang/RemapData/GEM"
	GEM="$WORKDIR/gem"
	MAPDIR="$WORKDIR/maps"
	XMLDIR="$WORKDIR/xml"

    SCRIP="/home/songshunqiang/RemapData/SCRIP/gen_scrip_gemwts.sh"
    XMLMETA="$WORKDIR/meta"

	if [[ $# == 3 ]]
	then
		mode=$3
	fi

	if [[ $mode == "test" ]]
	then
		echo "${GEM} ${MAPDIR}/${SRCNAME}_${DSTNAME}.nc ${XMLDIR}/${SRCNAME}_${DSTNAME}.xml ${MAPDIR}/${SRCNAME}_${DSTNAME}.order2.nc"
		#echo "${GEM} ${MAPDIR}/${DSTNAME}_${SRCNAME}.nc ${XMLDIR}/${DSTNAME}_${SRCNAME}.xml ${MAPDIR}/${DSTNAME}_${SRCNAME}.order2.nc"
	else
        echo ""
        echo "Work Begin"
        if [[ -e "${MAPDIR}/${SRCNAME}_${DSTNAME}.nc" ]]
        then
            echo "We have SCRIP weights from ${SRCNAME} to ${DSTNAME}"
        else
            echo "Generate SCRIP weights from ${SRCNAME} to ${DSTNAME}"
            time ${SCRIP} ${SRCNAME} ${DSTNAME}
            echo "Generate XML meta for SCRIP weights"
            #time ${XMLMETA} ${MAPDIR}/${SRCNAME}_${DSTNAME}.nc ${XMLDIR}/${SRCNAME}_${DSTNAME}.xml
            ${XMLMETA} ${MAPDIR}/${SRCNAME}_${DSTNAME}.nc ${XMLDIR}/${SRCNAME}_${DSTNAME}.xml
        fi
        echo "Generate GEM weights based on SCRIP weights from ${SRCNAME} to ${DSTNAME}"
		time ${GEM} ${MAPDIR}/${SRCNAME}_${DSTNAME}.nc ${XMLDIR}/${SRCNAME}_${DSTNAME}.xml ${MAPDIR}/${SRCNAME}_${DSTNAME}.order2.nc &> ${MAPDIR}/${SRCNAME}_${DSTNAME}.order2.log
        echo "Work Done"
        echo ""
		#time ${GEM} ${MAPDIR}/${DSTNAME}_${SRCNAME}.nc ${XMLDIR}/${DSTNAME}_${SRCNAME}.xml ${MAPDIR}/${DSTNAME}_${SRCNAME}.order2.nc &> ${MAPDIR}/${DSTNAME}_${SRCNAME}.order2.log
	fi
fi

