select site_name, country, pred_site_id, count(flight_id) as flights, sum(distance) as sum_dist, AsText(location) 
from sites right outer join flights on sites.site_id = flights.site_id 
group by site_name, country, pred_site_id, location 
order by sum_dist desc;


select site_name as site, generation as gen, count(generation), min(validation_error) as minerr, max(num_features) as maxfeat, min(train_time) as mintime, max(train_time) as maxtrain, max(train_time_prod) as prod 
from trained_solutions right outer join pred_sites on  trained_solutions.pred_site_id = pred_sites.pred_site_id
group by site_name, generation order by site_name, generation asc;


select site_name, generation, configuration, num_flight, max_dist, avg_dist, max_dur, avg_dur, validation_error, train_time, train_time_prod, num_samples_prod, num_features from trained_solutions inner join pred_sites on trained_solutions.pred_site_id = pred_sites.pred_site_id where avg_dur >= 0 order by pred_sites.pred_site_id, generation;


