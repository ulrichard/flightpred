# On the gfs production server the 1 deg grib data has gaps and corrupt files. 
# This script tries to fetch data that I couln't acquire before. Just in cast it has been fixed in the meantime.

rm get_problematic_days_weather.log

# missing files
# -> Response returned with status code 404
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2007-06-29' --end-date '2007-06-29' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2007-06-30' --end-date '2007-06-30' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2008-01-09' --end-date '2008-01-09' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2008-08-21' --end-date '2008-08-21' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-06-27' --end-date '2009-06-27' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-06-28' --end-date '2009-06-28' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-01-06' --end-date '2009-01-06' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-11-05' --end-date '2009-11-05' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-11-06' --end-date '2009-11-06' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-04-15' --end-date '2009-04-15' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-12-10' --end-date '2009-12-10' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-12-11' --end-date '2009-12-11' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-12-12' --end-date '2009-12-12' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-12-13' --end-date '2009-12-13' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-06-27' --end-date '2009-06-27' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2009-08-30' --end-date '2009-08-30' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2010-02-10' --end-date '2010-02-10' | tee -a get_problematic_days_weather.log


# corrupt ?
# -> GRIB_API ERROR   :  Error in accessor_add_dependency: cannot find identifier grib_expression_class_accessor.c at line 147: assertion failure Assert(observed)
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2008-01-01' --end-date '2008-01-01' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2008-01-10' --end-date '2008-01-10' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2008-01-12' --end-date '2008-01-12' | tee -a get_problematic_days_weather.log
../build/src/train/flightpred_train --db-user flightpred --db-password flightpred --get-weather --start-date '2008-01-28' --end-date '2008-01-28' | tee -a get_problematic_days_weather.log


