
CREATE TABLE contests
(
	contest_id SERIAL PRIMARY KEY,
	contest_name   varchar(50)  NOT NULL,
	website        varchar(256) NOT NULL
);
INSERT INTO contests (contest_name, website) values ('onlinecontest', 'http://www.onlinecontest.org');INSERT INTO contests (contest_name, website) values ('xcontest',      'http://www.xcontest.org');

CREATE TABLE pilots
(
	pilot_id SERIAL PRIMARY KEY,
	pilot_name   varchar(50) NOT NULL,
	country      varchar(5)  NOT NULL
);

CREATE TABLE sites
(
	site_id      SERIAL PRIMARY KEY,
	pred_site_id int,
	site_name    varchar(50) NOT NULL,
	country      varchar(5)  NOT NULL,
	CONSTRAINT FK_pred_site_id FOREIGN KEY(pred_site_id) REFERENCES pred_sites (pred_site_id)
);
SELECT AddGeometryColumn('sites', 'location', 4326, 'POINT', 2);
CREATE INDEX sites_by_location ON sites USING GIST(location);

CREATE TABLE flights
(
	flight_id SERIAL PRIMARY KEY,
	external_id  int,
	pilot_id     int          NOT NULL,
	contest_id   int          NOT NULL,
	site_id      int          NULL,
	flight_date  date         NOT NULL,
	igc_file     varchar(256) NULL,
	distance     real         NOT NULL,
	score	     int          NOT NULL,
	duration     real,
	CONSTRAINT FK_pilot_id     FOREIGN KEY(pilot_id)     REFERENCES pilots     (pilot_id)   ON DELETE CASCADE,	CONSTRAINT FK_contest_id   FOREIGN KEY(contest_id)   REFERENCES contests   (contest_id) ON DELETE CASCADE,	CONSTRAINT FK_site_id      FOREIGN KEY(site_id)      REFERENCES sites      (site_id)
);

CREATE TABLE pred_sites
(
	pred_site_id SERIAL PRIMARY KEY,
	site_name    varchar(50) NOT NULL,
	country      varchar(5)
);
SELECT AddGeometryColumn('pred_sites','location',4326,'POINT',2);
CREATE UNIQUE INDEX predSitesByName ON pred_sites(site_name);
INSERT INTO pred_sites (site_name, location, country) values ('Fiesch',      GeomFromText('POINT(8.1145333332485592 46.411933333078998)', 4326), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Grindelwald', GeomFromText('POINT(8.0552869234381106 46.656613323997803)', 4326), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Niesen',      GeomFromText('POINT(7.6476388888888902 46.644222222434202)', 4326), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Rothenflue',  GeomFromText('POINT(8.7019931164065607 47.018337296086003)', 4326), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Engelberg',   GeomFromText('POINT(8.4053166664971304 46.840041666560701)', 4326), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Uetliberg',   GeomFromText('POINT(8.5012333333333299 47.323266666666697)', 4326), 'CH');

CREATE TABLE trained_solutions
(
	train_sol_id    SERIAL PRIMARY KEY,
	pred_site_id   int,
	configuration  text,
	svm_num_flight oid,
	svm_max_dist   oid,
	svm_avg_dist   oid,
	svm_max_dur    oid,
	svm_avg_dur    oid,
	normalizer     oid,
	score          real,
	train_time     real,
	generation     int,
	CONSTRAINT FK_pred_site_id FOREIGN KEY(pred_site_id) REFERENCES pred_sites(pred_site_id) ON DELETE CASCADE
);

CREATE TABLE weather_models
(
	model_id SERIAL PRIMARY KEY,
	model_name VARCHAR(10)   NOT NULL,
	grid_step  real,
	url_past   VARCHAR(512)  NOT NULL,
	url_future VARCHAR(512)  NOT NULL	
);

INSERT INTO weather_models (model_name, grid_step, url_past, url_future) values ('GFS', 2.5, 'nomad3.ncep.noaa.gov/pub/reanalysis-2/6hr/pgb/', 'http://nomad3.ncep.noaa.gov/pub/nomad1-raid1/gfs_master/');
CREATE TABLE weather_pred
(
	weather_pred_id SERIAL PRIMARY KEY,
	model_id    int                          NOT NULL,
	pred_time   timestamp  without time zone NOT NULL,
	level       real                         NOT NULL,
	parameter   varchar(10)                  NOT NULL,
	value       real                         NOT NULL
);
SELECT AddGeometryColumn('weather_pred', 'location', 4326, 'POINT', 2);
CREATE INDEX weatherByTime         ON weather_pred (pred_time);
CREATE INDEX weatherByLvLParamTime ON weather_pred (level, parameter, pred_time);
CREATE INDEX weatherByLocation     ON weather_pred USING GIST (location);




