mkdir ../backup
#sudo -u postgres pg_dump                              flightpred > ../backup/dbdump.sql

sudo -u postgres pg_dump -a -b -t pred_sites          flightpred > ../backup/pred_sites.sql
sudo -u postgres pg_dump -a -b -t contests            flightpred > ../backup/contests.sql
sudo -u postgres pg_dump -a -b -t pilots              flightpred > ../backup/pilots.sql
sudo -u postgres pg_dump -a -b -t sites               flightpred > ../backup/sites.sql
sudo -u postgres pg_dump -a -b -t flights             flightpred > ../backup/flights.sql
sudo -u postgres pg_dump -a -b -t trained_solutions   flightpred > ../backup/trained_solutions.sql
sudo -u postgres pg_dump -a -b -t weather_models      flightpred > ../backup/weather_models.sql
sudo -u postgres pg_dump -a -b -t weather_pred        flightpred > ../backup/weather_pred.sql
sudo -u postgres pg_dump -a -b -t weather_pred_future flightpred > ../backup/weather_pred_future.sql
sudo -u postgres pg_dump -a -b -t pred_ignore		  flightpred > ../backup/pred_ignore.sql
sudo -u postgres pg_dump -a -b -t flight_pred         flightpred > ../backup/flight_pred.sql

