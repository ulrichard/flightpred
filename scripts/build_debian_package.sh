mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.3
cd flightpred-0.0.3
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-0.0.3
dput ppa:richi-paraeasy/ppa ./flightpred_0.0.3_source.changes
cd ..
rm -r builddeb

