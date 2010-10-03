<?php

header('Content-type: application/xml');

// Connecting, selecting database
$dbconn = pg_connect("host=localhost dbname=flightpred user=flightpred password=flightpred")
    or die('Could not connect: ' . pg_last_error());


$query = "select distinct pred_day from flight_pred where pred_day >= date_trunc('day', now())";
$days = pg_query($query) or die('Query failed: ' . pg_last_error());

echo "<flightpred>\n";
while($day = pg_fetch_assoc($days)) 
{
    echo "\t<day val='{$day['pred_day']}'>\n";


	// Performing SQL query
	$query = 'select pred_site_id, site_name, country, astext(location) as loc from pred_sites;';
	$sites = pg_query($query) or die('Query failed: ' . pg_last_error());

	while($site = pg_fetch_assoc($sites)) 
	{
		echo "\t\t<site name='{$site['site_name']}' location='{$site['loc']}'>\n";
	 
		$query = "select num_flight, max_dist, avg_dist, max_dur, avg_dur  from flight_pred where pred_day='{$day['pred_day']}' and pred_site_id={$site['pred_site_id']} order by calculated desc limit 1;";
		$result = pg_query($query) or die('Query failed: ' . pg_last_error());
		$results = pg_fetch_assoc($result);
		echo "\t\t\t<num_flight>{$results['num_flight']}</num_flight>\n";
		echo "\t\t\t<max_dist>{$results['max_dist']}</max_dist>\n";
		echo "\t\t\t<avg_dist>{$results['avg_dist']}</avg_dist>\n";
		echo "\t\t\t<max_dur>{$results['max_dur']}</max_dur>\n";
		echo "\t\t\t<avg_dur>{$results['avg_dur']}</avg_dur>\n";

		echo "\t\t</site>\n";

		pg_free_result($result);
	}

	echo "\t</day>\n";
}

echo "</flightpred>\n";


// Free resultset

pg_free_result($sites);

// Closing connection
pg_close($dbconn);
?>

