SHELL=/bin/sh


0 6 * * * /usr/bin/flightpred-train --db-password flightpred --get-future-weather --forecast --db-maintenance

