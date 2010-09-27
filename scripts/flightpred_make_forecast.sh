#!/bin/sh
# create debian pakage for json_spirit

/usr/bin/flightpred_train --db-user flightpred --db-password flightpred --get-future-weather --forecast --db-maintenance > /tmp/flightpred.log
