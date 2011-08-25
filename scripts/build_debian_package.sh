#!/bin/sh
# build a debian sourcepackage and upload it to the launchpad ppa
export GPGKEY=DA94BB53
export DEBEMAIL="richi@paraeasy.ch"
export DEBFULLNAME="Richard Ulrich"
killall -q gpg-agent
eval $(gpg-agent --daemon)
rm -r builddeb
mkdir builddeb
cd builddeb
for DISTRIBUTION in maverick natty oneiric
do
	FLIGHTPREDVERSIONSTR=0.0.28~${DISTRIBUTION}
	svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-$FLIGHTPREDVERSIONSTR
	cd flightpred-$FLIGHTPREDVERSIONSTR
	sed -i  -e "s/jaunty/${DISTRIBUTION}/g" -e "s/karmic/${DISTRIBUTION}/g" -e "s/maverick/${DISTRIBUTION}/g" -e "s/natty/${DISTRIBUTION}/g" -e "s/oneiric/${DISTRIBUTION}/g" debian/changelog
	rm -r 3rd-party/dclib/docs/*
	rm -r 3rd-party/dclib/examples/*
	rm -r 3rd-party/ggl/libs/*
	rm -r 3rd-party/ggl/other/*
	dpkg-buildpackage -rfakeroot -S
	cd ..
	rm -r flightpred-$FLIGHTPREDVERSIONSTR
	dput ppa:richi-paraeasy/ppa ./flightpred_${FLIGHTPREDVERSIONSTR}_source.changes
done
killall -q gpg-agent
