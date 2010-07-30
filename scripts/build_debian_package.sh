mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.5
cd flightpred-0.0.5
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-0.0.5
dput ppa:richi-paraeasy/ppa ./flightpred_0.0.5_source.changes
cd ..
rm -r builddeb

