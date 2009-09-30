#!/bin/sh
# install required debian packages for ubuntu

sudo apt-get install postgresql postgresql-contrib postgresql-client postgresql-server-dev-8.3 postgresql-8.3-postgis postgis proj libpqxx-dev
sudo apt-get install libboost1.37-dev libboost-program-options1.37-dev libboost-date-time1.37-dev libboost-regex1.37-dev libboost-filesystem1.37-dev libboost-program-options1.37-dev
#sudo apt-get install libboost-dev libboost-program-options-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-program-options-dev
sudo apt-get install libtorch3-dev liblog4cpp5-dev libreadline5-dev libjasper1 libpqxx-dev
