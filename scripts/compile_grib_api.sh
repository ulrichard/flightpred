#!/bin/sh
# compile and install grib_api (maybe create a deb package later)

sudo apt-get install libjasper-dev

sudo mkdir /usr/local/grib_api
cd ../3rd-party/grib_api
./configure --prefix=/usr/local/grib_api
make
make check
sudo make install
