#!/bin/sh
# install required debian packages for ubuntu

sudo apt-get install postgresql-8.4 postgresql-contrib-8.4 postgresql-client-8.4 postgresql-server-dev-8.4 postgresql-8.4-postgis postgis proj libpqxx-dev libgrib-api-dev
#sudo apt-get install libboost1.37-dev libboost-program-options1.37-dev libboost-date-time1.37-dev libboost-regex1.37-dev libboost-filesystem1.37-dev libboost-program-options1.37-dev
sudo apt-get install libboost-dev libboost-program-options-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-program-options-dev libboost-iostreams-dev libboost-test-dev
#sudo apt-get install liblog4cpp5-dev libreadline5-dev libjasper1
sudo apt-get install libcoyotl-dev libevocosm-dev debhelper
sudo apt-get install libwt-dev libwthttp-dev libwtext-dev libwtfcgi-dev

