rm -r builddeb
mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.8
cd flightpred-0.0.8
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-0.0.8
dput ppa:richi-paraeasy/ppa ./flightpred_0.0.8_source.changes

