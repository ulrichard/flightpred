<?php

header('Content-type: application/xml');

echo "<rss version=\"2.0\">\n";
echo "\t<channel>\n";
echo "\t\t<title>Flightpred</title>\n";
echo "\t\t<description>A system that shall predict good days and locations for cross country free flying such as paragliding by comparing current weather predictions with statistics about past weather predictions and flights from online contests. </description>\n";
echo "\t\t<language>en</language>\n";
echo "\t\t<link>http://flightpred.homelinux.org</link>\n";
echo "\t\t<lastBuildDate>Sat, 1 Jan 2000 00:00:00 GMT</lastBuildDate>\n";

// Connecting, selecting database
$dbconn = pg_connect("host=localhost dbname=flightpred user=flightpred password=flightpred")
    or die('Could not connect: ' . pg_last_error());


$query = "select distinct pred_day from flight_pred where pred_day >= date_trunc('day', now()) order by pred_day desc";
$days = pg_query($query) or die('Query failed: ' . pg_last_error());

while($day = pg_fetch_assoc($days)) 
{
    echo "\t\t<item>\n";
	echo "\t\t\t<title>{$day['pred_day']}</title>\n";	
    echo "\t\t\t<link>http://flightpred.homelinux.org</link>\n";
    echo "\t\t\t<pubDate>Sat, 1. Jan 2000 00:00:00 GMT</pubDate>\n";
    echo "\t\t\t<guid>01012000-000000</guid>\n";
	echo "\t\t\t<description>";
	echo "\t\t\t\t<b>free flight prediction for {$day['pred_day']}</b>\n";	
	echo "\t\t\t\t<table><th><td>Site</td><td>dist</td></th>\n";	



	// Performing SQL query
	$query = 'select pred_site_id, site_name, country, astext(location) as loc from pred_sites;';
	$sites = pg_query($query) or die('Query failed: ' . pg_last_error());

	while($site = pg_fetch_assoc($sites)) 
	{		 
		$query = "select max_dist, calculated from flight_pred where pred_day='{$day['pred_day']}' and pred_site_id={$site['pred_site_id']} order by calculated desc limit 1;";
		$result = pg_query($query) or die('Query failed: ' . pg_last_error());
		$results = pg_fetch_assoc($result);
		if($results['max_dist'] > 0.0)
		{
			echo "\t\t\t\t<tr><td>{$site['site_name']}</td>\n";
			echo "<td>{$results['max_dist']}</td></tr>\n";
		}
		pg_free_result($result);
	}

	echo "</table></description>\n";
	echo "\t\t</item>\n";
}

echo "\t</channel>\n";
echo "</rss>\n";


// Free resultset

pg_free_result($sites);

// Closing connection
pg_close($dbconn);
?>

