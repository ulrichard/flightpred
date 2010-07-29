mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.4
cd flightpred-0.0.4
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-0.0.4
dput ppa:richi-paraeasy/ppa ./flightpred_0.0.4_source.changes
cd ..
rm -r builddeb

