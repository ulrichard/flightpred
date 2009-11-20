#!/bin/sh
# set up the flightpred database

sudo apt-get install postgresql postgresql-contrib postgresql-client postgresql-server-dev-8.4 postgis proj libpqxx-dev

sudo -u postgres dropdb   -e flightpred
sudo -u postgres createdb -e flightpred
sudo -u postgres createlang plpgsql flightpred
sudo -u postgres psql -e -a -d flightpred -f /usr/share/postgresql-8.4-postgis/lwpostgis.sql
sudo -u postgres psql -e -a -d flightpred -f /usr/share/postgresql-8.4-postgis/spatial_ref_sys.sql
sudo -u postgres psql -e -a -d flightpred -f create_flightpred_db.sql



