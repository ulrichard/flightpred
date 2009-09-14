#!/bin/sh
# simple script to download current forecast

cdate=$(date +%04Y%02m%02d)00
echo $cdate
./get_gfs.pl data $cdate 0 72 3 HGT:TMP surface:850_mb:750_mb:500_mb . 
