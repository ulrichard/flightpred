mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.1
cd flightpred-0.0.1
dpkg-buildpackage -rfakeroot
rm -r flightpred-0.0.1
