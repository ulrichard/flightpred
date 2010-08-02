mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.6
cd flightpred-0.0.6
dpkg-buildpackage -rfakeroot -S
cd ..
rm -r flightpred-0.0.6
cd ..
rm -r builddeb

