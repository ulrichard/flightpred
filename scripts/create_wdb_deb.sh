#!/bin/sh
# create debian pakage for the wdb

sudo apt-get install svn-buildpackage libgeos-dev postgresql-8.3-postgis xmlto

#build the core wdb
cd ../3rd-party/wdb/src/wdb
./autogen.sh
./configure
make
#make dist
# build the deb package for the core wdb
mv debian_files debian
dpkg-buildpackage -rfakeroot
#cd ../../deb
#mkdir tarballs
#cp ../src/wdb/wdb-0.9.4.tar.gz tarballs/wdb.0.9.4.orig.tar.gz
#cd wdb
#svn-buildpackage -us -uc -rfakeroot

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


