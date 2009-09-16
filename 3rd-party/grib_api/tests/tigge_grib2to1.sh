#!/bin/sh
# Copyright 2005-2007 ECMWF
# 
# Licensed under the GNU Lesser General Public License which
# incorporates the terms and conditions of version 3 of the GNU
# General Public License.
# See LICENSE and gpl-3.0.txt for details.

# --- test grib edition 2 to 1 conversions with tigge data
# ---   tests some keys remain the same after the conversion
# ---   also test the values

. ./include.sh

#set -eax 

dir="${data_dir}/tigge/"
temp="temp.grib1_"

# --- exclude babj as the way they encode grib 2 makes it
# ---   problematic to convert to grib 1
exclusion_pattern="nothing"

for file in `ls ${dir}/tigge_*.grib`
do

exclude=`echo $file | awk " /$exclusion_pattern/ {print \"found\";} "`

if [ -z "$exclude" ]; then

	rm -f ${temp} || true

	${tools_dir}grib_set -s editionNumber=1 ${file} ${temp} 2> /dev/null > /dev/null


	grib1Statistics=`${tools_dir}grib_get -p numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${temp}` 
	grib2Statistics=`${tools_dir}grib_get -p numberOfValues,numberOfPoints,max,min,average,numberOfMissing ${file}` 

	if [ "$grib1Statistics" != "$grib2Statistics" ]
	then 
	  exit 1
	fi

	${tools_dir}grib_compare -P -c values ${temp} ${file} 2> /dev/null > /dev/null

	rm -f ${temp} || true
fi

done

