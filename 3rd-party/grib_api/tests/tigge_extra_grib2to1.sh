#!/bin/sh
# Copyright 2005-2007 ECMWF
# 
# Licensed under the GNU Lesser General Public License which
# incorporates the terms and conditions of version 3 of the GNU
# General Public License.
# See LICENSE and gpl-3.0.txt for details.

# --- test grib edition 2 to 1 conversions with tigge data

# --- check converting to grib 1 and then back again to 2
# ---   gives comparable results to the source grib 2 input file

. ./include.sh


dir="${data_dir}/tigge/"
temp1="temp.grib1_"
temp2="temp.grib2_"

# ---   problematic to convert to grib 1
exclusion_pattern="st|ttr|ci|cap|tcw|ssr|str|skt"

blacklist="mn2t6 mx2t6 orog tcw ssr str skt pt sm ttr cape ci"

for file in `ls ${dir}/tigge_*.grib`
do

exclude=`echo $file | awk " /$exclusion_pattern/ {print \"found\";} "`

if [ -z "$exclude" ]; then

	rm -f ${temp1} ${temp2} || true

	# --- convert from edition 2 to 1 and back
	${tools_dir}grib_set -s editionNumber=1 ${file} ${temp1} 2> /dev/null > /dev/null
	${tools_dir}grib_set -s productionStatusOfProcessedData=5,editionNumber=2 ${temp1} ${temp2} 2> /dev/null > /dev/null

	# --- packingError is not comparable when converting from grib 2 to 1
	${tools_dir}grib_compare -P -c values ${temp2} ${file} 2> /dev/null > /dev/null

	# --- check shortName is kept
	name1=`${tools_dir}grib_get -p shortName ${file}`
	name2=`${tools_dir}grib_get -p shortName ${temp2}`

    is_blacklisted=`echo $blacklist | awk " /$name1/ {print \"found\";} "`
	if [ -z "${is_blacklisted}" ] ; then
	if [ "$name1" != "$name2" ]; then
		echo "ERROR: 'shortName' not matching after conversion."
		#echo $name1 >> params
		read -p "Press enter key to continue"
		#exit 1
	fi
	fi

	rm -f ${temp1} ${temp2} || true

fi

done

