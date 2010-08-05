#!/bin/sh
# set up the flightpred database

sudo apt-get install postgresql postgresql-contrib postgresql-client postgresql-server-dev-8.4 postgis proj libpqxx-dev

if ! grep -q "^flightpred:" /etc/passwd ; then
    adduser --gecos 0 --disabled-password --disabled-login flightpred 
fi

sudo -u postgres psql -e -a -c "DROP DATABASE IF EXISTS flightpred"
sudo -u postgres psql -e -a -c "CREATE USER flightpred WITH PASSWORD 'flightpred' SUPERUSER"
sudo -u postgres createdb -e flightpred
sudo -u postgres createlang plpgsql flightpred
sudo -u postgres psql -e -a -d flightpred -f /usr/share/postgresql/8.4/contrib/postgis.sql
sudo -u postgres psql -e -a -d flightpred -f /usr/share/postgresql/8.4/contrib/spatial_ref_sys.sql
sudo -u postgres psql -e -a -d flightpred -f create_flightpred_db.sql
#sudo -u postgres psql -e -a -c "ALTER ROLE flightpred NOSUPERUSER"

#easier handling for the current user
sudo -u postgres createuser --superuser $USER
sudo -u postgres psql -e -a -c "GRANT ALL PRIVILEGES ON DATABASE flightpred TO $USER"


