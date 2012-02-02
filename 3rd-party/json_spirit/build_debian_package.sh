#!/bin/sh
# build a debian sourcepackage and upload it to the launchpad ppa
export GPGKEY=DA94BB53
export DEBEMAIL="richi@paraeasy.ch"
export DEBFULLNAME="Richard Ulrich"
rm -r builddeb
mkdir builddeb
cd builddeb
for DISTRIBUTION in precise # oneiric natty maverick 
do
	JSONSPIRITVERSIONSTR=4.2.1~${DISTRIBUTION}
		svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk/3rd-party/json_spirit/ libjsonspirit-$JSONSPIRITVERSIONSTR
	cd libjsonspirit-$JSONSPIRITVERSIONSTR
	sed -i  -e "s/maverick/${DISTRIBUTION}/g" -e "s/natty/${DISTRIBUTION}/g" -e "s/oneiric/${DISTRIBUTION}/g" debian/changelog
	dpkg-buildpackage -rfakeroot -S
	cd ..
	rm -r libjsonspirit-$JSONSPIRITVERSIONSTR
	dput ppa:richi-paraeasy/ppa ./libjsonspirit_${JSONSPIRITVERSIONSTR}_source.changes
done




