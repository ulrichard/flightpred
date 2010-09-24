# /etc/cron.d/flightpred: crontab entries for the flightpred package

SHELL=/bin/sh
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
MAILTO=root

0 4,15 * * * root /usr/bin/flightpred_train --db-user flightpred --db-password flightpred --get-future-weather --forecast --db-maintenance
