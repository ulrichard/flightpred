#!/bin/sh
# build a debian sourcepackage and upload it to the launchpad ppa
export VersionStr=0.0.11
export GPGKEY=DA94BB53
export DEBEMAIL="richi@paraeasy.ch"
export DEBFULLNAME="Richard Ulrich"
killall -q gpg-agent
eval $(gpg-agent --daemon)
rm -r builddeb
mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-$(VersionStr)
cd flightpred-$(VersionStr)
rm -r 3rd-party/dclib/docs/*
rm -r 3rd-party/dclib/examples/*
rm -r 3rd-party/ggl/libs/*
rm -r 3rd-party/ggl/other/*
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-$(VersionStr)
dput ppa:richi-paraeasy/ppa ./flightpred_$(VersionStr)_source.changes
killall -q gpg-agent
