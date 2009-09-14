#!/bin/sh
# simple script to download 4x daily V winds at 10mb
# from the R2 archive

mkdir ./past
rm -r ./past/*
cd ./past

set -x
date=200401
enddate=200908
while [ $date -le $enddate ]
do
     url="http://nomad3.ncep.noaa.gov/pub/reanalysis-2/6hr/pgb/pgb.$date"
     ../get_inv.pl "${url}.inv" | grep ":VGRD:" | grep ":10 mb" | \
     ../get_grib.pl "${url}" pgb.$date
     date=$(($date + 1))
     if [ $(($date % 100)) -eq 13 ] ; then
         date=$(($date - 12 + 100));
     fi
done
