
sudo apt-get remove flightpred -y
sudo -u postgres dropdb flightpred
sudo -u postgres dropuser www-data
sudo -u postgres dropuser flightpred
sudo rm /etc/flightpred.conf

sudo dpkg -i ../../flightpred_0.0.1_i386.deb
