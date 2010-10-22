<?php

$format = isset($_GET["format"]) ? $_GET["format"] : "xml";

if($format == "json")
    header('Content-type: application/json');
else
	header('Content-type: application/xml');

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Datum aus Vergangenheit
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT"); // immer geändert
header("Cache-Control: no-store, no-cache, must-revalidate");  // HTTP/1.1
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");                          // HTTP/1.0


// Connecting, selecting database
$dbconn = pg_connect("host=localhost dbname=flightpred user=flightpred password=flightpred")
    or die('Could not connect: ' . pg_last_error());


$query = "select distinct pred_day from flight_pred where pred_day >= date_trunc('day', now())";
$days = pg_query($query) or die('Query failed: ' . pg_last_error());

if($format == "json")
	echo "{\"days\":[\n";
else
	echo "<flightpred>\n";

while($day = pg_fetch_assoc($days)) 
{
	if($format == "json")
    	echo "\t{\"day\":\"{$day['pred_day']}\",\"sites\":[\n";
	else
    	echo "\t<day val='{$day['pred_day']}'>\n";


	// Performing SQL query
	$query = 'select pred_site_id, site_name, country, astext(location) as loc from pred_sites;';
	$sites = pg_query($query) or die('Query failed: ' . pg_last_error());

	while($site = pg_fetch_assoc($sites)) 
	{	 
		$query = "select num_flight, max_dist, avg_dist, max_dur, avg_dur  from flight_pred where pred_day='{$day['pred_day']}' and pred_site_id={$site['pred_site_id']} order by calculated desc limit 1;";
		$result = pg_query($query) or die('Query failed: ' . pg_last_error());
		$results = pg_fetch_assoc($result);

		if($format == "json")
		{
			if($results['num_flight'] || $results['max_dist'] || $results['max_dur'])
			{
				echo "\t\t{\"site\":\"{$site['site_name']}\"";
				echo ",\"location\":\"{$site['loc']}\"";
				if($results['num_flight'])
					echo ",\"num_flight\":{$results['num_flight']}";
				if($results['max_dist'])
					echo ",\"max_dist\":{$results['max_dist']}";
				if($results['avg_dist'])
					echo ",\"avg_dist\":{$results['avg_dist']}";
				if($results['max_dur'])
					echo ",\"max_dur\":{$results['max_dur']}";
				if($results['avg_dur'])
					echo ",\"avg_dur\":{$results['avg_dur']}";
				echo "}\n";
			}
		}
		else
		{
			echo "\t\t<site name='{$site['site_name']}' location='{$site['loc']}'>\n";
			echo "\t\t\t<num_flight>{$results['num_flight']}</num_flight>\n";
			echo "\t\t\t<max_dist>{$results['max_dist']}</max_dist>\n";
			echo "\t\t\t<avg_dist>{$results['avg_dist']}</avg_dist>\n";
			echo "\t\t\t<max_dur>{$results['max_dur']}</max_dur>\n";
			echo "\t\t\t<avg_dur>{$results['avg_dur']}</avg_dur>\n";
			echo "\t\t</site>\n";
		}

		pg_free_result($result);
	}

	if($format == "json")
		echo "\t]}\n";
	else
		echo "\t</day>\n";
}

if($format == "json")
	echo "]}\n";
else
	echo "</flightpred>\n";


// Free resultset

pg_free_result($sites);

// Closing connection
pg_close($dbconn);
?>

