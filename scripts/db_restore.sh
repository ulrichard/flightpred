
sudo -u flightpred psql < ../backup/pred_sites.sql
sudo -u flightpred psql < ../backup/contests.sql
sudo -u flightpred psql < ../backup/pilots.sql
sudo -u flightpred psql < ../backup/sites.sql
sudo -u flightpred psql < ../backup/flights.sql

sudo -u flightpred echo "delete from trained_solutions" | psql
sudo -u flightpred vacuumlo flightpred
sudo -u flightpred psql < ../backup/trained_solutions.sql

sudo -u flightpred psql < ../backup/weather_models.sql
sudo -u flightpred psql < ../backup/weather_pred.sql
sudo -u flightpred psql < ../backup/weather_pred_future.sql
sudo -u flightpred psql < ../backup/pred_ignore.sql

