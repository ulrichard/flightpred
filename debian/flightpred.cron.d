# /etc/cron.d/flightpred: crontab entries for the flightpred package

SHELL=/bin/sh
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
MAILTO=richi@paraeasy.ch
HOME=/tmp

0 */8 * * * flightpred /usr/bin/flightpred_train --db-user flightpred --db-password flightpred --get-future-weather --forecast --db-maintenance
