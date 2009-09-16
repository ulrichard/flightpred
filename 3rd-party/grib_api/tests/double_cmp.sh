#!/bin/sh
# Copyright 2005-2007 ECMWF
# 
# Licensed under the GNU Lesser General Public License which
# incorporates the terms and conditions of version 3 of the GNU
# General Public License.
# See LICENSE and gpl-3.0.txt for details.

# --- test on the grid_ieee type of packing
# ---   check if retrieving data through grib_get_double_array and
# ---   grib_get_double_element provides the same result

. ./include.sh

infile=${data_dir}/grid_ieee.grib

if [ ! -f ${infile} ]
then
  echo no data to test
  exit 1
fi

${test_dir}/double_cmp ${infile}
