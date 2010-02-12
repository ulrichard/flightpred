#!/bin/sh
# create debian pakage for json_spirit


cd ../3rd-party/json_spirit
rm -r build
mkdir build
cd build
cmake ../
make
cd ..
dpkg-buildpackage -rfakeroot
