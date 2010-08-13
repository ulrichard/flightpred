select pred_site_id as site, generation as gen, count(generation), min(validation_error) as minerr, max(num_features) as maxfeat, min(train_time) as mintime, max(train_time) as maxtrain, max(train_time_prod) as prod from trained_solutions group by pred_site_id, generation order by pred_site_id, generation asc;


