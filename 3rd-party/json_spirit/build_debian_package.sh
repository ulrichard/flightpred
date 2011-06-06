#!/bin/sh
# build a debian sourcepackage and upload it to the launchpad ppa
JSONSPIRITVERSIONSTR=4.2.1~natty
export GPGKEY=DA94BB53
export DEBEMAIL="richi@paraeasy.ch"
export DEBFULLNAME="Richard Ulrich"
killall -q gpg-agent
eval $(gpg-agent --daemon)
rm -r builddeb
mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk/3rd-party/json_spirit/ libjsonspirit-$JSONSPIRITVERSIONSTR
cd libjsonspirit-$JSONSPIRITVERSIONSTR
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r libjsonspirit-$JSONSPIRITVERSIONSTR
dput ppa:richi-paraeasy/ppa ./libjsonspirit_${JSONSPIRITVERSIONSTR}_source.changes
killall -q gpg-agent
