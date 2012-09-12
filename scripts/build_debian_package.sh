#!/bin/sh
# build a debian sourcepackage and upload it to the launchpad ppa
rm -r builddeb
mkdir builddeb
cd builddeb
for DISTRIBUTION in precise #oneiric # natty maverick 
do
	FLIGHTPREDVERSIONSTR=0.0.35~${DISTRIBUTION}
	svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-$FLIGHTPREDVERSIONSTR
	cd flightpred-$FLIGHTPREDVERSIONSTR
	sed -i  -e "s/maverick/${DISTRIBUTION}/g" -e "s/natty/${DISTRIBUTION}/g" -e "s/oneiric/${DISTRIBUTION}/g" debian/changelog
	rm -r 3rd-party/dclib/docs/*
	rm -r 3rd-party/dclib/examples/*
	rm -r 3rd-party/ggl/libs/*
	rm -r 3rd-party/ggl/other/*
	dpkg-buildpackage -rfakeroot -S
	cd ..
	rm -r flightpred-$FLIGHTPREDVERSIONSTR
	dput ppa:richi-paraeasy/ppa ./flightpred_${FLIGHTPREDVERSIONSTR}_source.changes
done
