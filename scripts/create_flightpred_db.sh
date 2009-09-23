#!/bin/sh
# set up the flightpred database

sudo apt-get install postgresql wdb

sudo -u postgres dropdb   -e flightpred
sudo -u postgres createdb -e flightpred
sudo -u postgres psql     -e -a -d flightpred -f create_flightpred_db.sql