mkdir builddeb
cd builddeb
svn export https://flightpred.svn.sourceforge.net/svnroot/flightpred/trunk flightpred-0.0.2
cd flightpred-0.0.2
dpkg-buildpackage -rfakeroot -S
rm -r flightpred-0.0.2
dput ppa:richi-paraeasy/ppa ./flightpred_0.0.2_source.changes
