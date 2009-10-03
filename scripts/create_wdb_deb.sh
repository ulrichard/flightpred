#!/bin/sh
# create debian pakage for the wdb

sudo apt-get install svn-buildpackage libgeos-dev postgresql-8.3-postgis xmlto

#build the core wdb
cd ../3rd-party/wdb/wdb
./autogen.sh
./configure --with-boost-date-time=mt --with-boost-regex=mt --with-boost-filesystem=mt --with-boost-thread=mt
make
#make dist
# build the deb package for the core wdb
rm -r debian
cp debian_files debian
dpkg-buildpackage -rfakeroot

#build the loader base
cd ../loader/loaderBase
./autogen.sh
#./configure
#make
#make dist
mv debian_files debian
dpkg-buildpackage -rfakeroot


#build the grib loader
cd ../loader/gribLoad
./autogen.sh
#./configure
#make
#make dist
mv debian_files debian
dpkg-buildpackage -rfakeroot


