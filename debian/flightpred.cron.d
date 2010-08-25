SHELL=/bin/sh


0 6 * * * flightpred /usr/bin/flightpred-train --db-user flightpred --db-password flightpred --get-future-weather --forecast --db-maintenance

