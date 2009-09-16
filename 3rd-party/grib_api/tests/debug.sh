#!/bin/sh
# Copyright 2005-2007 ECMWF
# 
# Licensed under the GNU Lesser General Public License which
# incorporates the terms and conditions of version 3 of the GNU
# General Public License.
# See LICENSE and gpl-3.0.txt for details.


. ./include.sh

for file in ${data_dir}/*.grib2 ${data_dir}/*.grib1
do
${tools_dir}grib_debug $file 2> /dev/null > /dev/null
done

