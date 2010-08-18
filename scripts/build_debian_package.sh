#!/bin/sh
# build a debian sourcepackage and upload it to the launchpad ppa
export VersionStr=0.0.10
export GPGKEY=DA94BB53
export DEBEMAIL="richi@paraeasy.ch"
export DEBFULLNAME="Richard Ulrich"
killall -q gpg-agent
eval $(gpg-agent --daemon)
rm -r builddeb
mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.10
cd flightpred-0.0.10
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-0.0.10
dput ppa:richi-paraeasy/ppa ./flightpred_0.0.10_source.changes
killall -q gpg-agent
