#!/bin/sh
# compile and install grib_api (maybe create a deb package later)

sudo apt-get install libjasper-dev
sudo apt-get install cdbs dh-buildinfo gfortran flex bison
sudo apt-get install libjpeg62-dev quilt

cd ../3rd-party/grib_api
dpkg-buildpackage -rfakeroot

