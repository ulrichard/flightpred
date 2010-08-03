rm -r builddeb
mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.7
cd flightpred-0.0.7
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-0.0.7
dput ppa:richi-paraeasy/ppa ./flightpred_0.0.7_source.changes

