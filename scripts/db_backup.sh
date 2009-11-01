mkdir ../backup
sudo -u postgres pg_dump flightpred > ../backup/dbdump.sql
