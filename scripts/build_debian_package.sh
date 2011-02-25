#!/bin/sh
# build a debian sourcepackage and upload it to the launchpad ppa
FLIGHTPREDVERSIONSTR=0.0.17~maverick
export GPGKEY=DA94BB53
export DEBEMAIL="richi@paraeasy.ch"
export DEBFULLNAME="Richard Ulrich"
killall -q gpg-agent
eval $(gpg-agent --daemon)
rm -r builddeb
mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-$FLIGHTPREDVERSIONSTR
cd flightpred-$FLIGHTPREDVERSIONSTR
rm -r 3rd-party/dclib/docs/*
rm -r 3rd-party/dclib/examples/*
rm -r 3rd-party/ggl/libs/*
rm -r 3rd-party/ggl/other/*
#ifeq ($(shell dpkg-vendor --derives-from Ubuntu && echo yes),yes)
  dpkg-buildpackage -rfakeroot -S
#else
#  dpkg-buildpackage -rfakeroot -S -d # injecting different package dependencies after the check
#endif
cd ..
rm -r flightpred-$FLIGHTPREDVERSIONSTR
dput ppa:richi-paraeasy/ppa ./flightpred_${FLIGHTPREDVERSIONSTR}_source.changes
killall -q gpg-agent
