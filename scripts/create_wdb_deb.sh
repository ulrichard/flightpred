#!/bin/sh
# create debian pakage for the wdb

sudo apt-get install svn-buildpackage

cd ../3rd-party/wdb/src/wdb
./autogen.sh
./configure
make
make dist

cd ../deb
mkdir tarballs
cp DIST tarballs/wdb.0.9.4.orig.tar.gz
cd wdb
svn-buildpackage -us -uc -rfakeroot

