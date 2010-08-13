select site_name as site, generation as gen, count(generation), min(validation_error) as minerr, max(num_features) as maxfeat, min(train_time) as mintime, max(train_time) as maxtrain, max(train_time_prod) as prod 
from trained_solutions right outer join pred_sites on  trained_solutions.pred_site_id = pred_sites.pred_site_id
group by site_name, generation order by site_name, generation asc;


