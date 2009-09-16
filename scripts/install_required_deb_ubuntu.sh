#!/bin/sh
# install required debian packages for ubuntu

sudo apt-get install postgresql postgresql-contrib postgresql-client postgresql-server-dev-8.3 postgresql-8.3-postgis postgis proj
sudo apt-get install libboost-dev libboost-program-options-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev
sudo apt-get libtorch3-dev liblog4cpp5-dev libreadline5-dev libjasper1 libpqxx-dev
