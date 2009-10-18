
CREATE TABLE pilots
(
	pilot_id SERIAL PRIMARY KEY,
	pilot_name   varchar(50) NOT NULL,
	country      varchar(5)  NOT NULL
);

CREATE TABLE contests
(
	contest_id SERIAL PRIMARY KEY,
	contest_name   varchar(50)  NOT NULL,
	website        varchar(256) NOT NULL
);

CREATE TABLE sites
(
	site_id      SERIAL PRIMARY KEY,
	pred_site_id int,
	site_name    varchar(50) NOT NULL,
	country      varchar(5)  NOT NULL
);
SELECT AddGeometryColumn('sites','location',-1,'POINT',2);
CREATE INDEX sites_by_location ON sites USING GIST ( location );

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
	score	     int          NOT NULL
);

CREATE TABLE pred_sites
(
	pred_site_id SERIAL PRIMARY KEY,
	site_name    varchar(50) NOT NULL,
	country      varchar(5)
);
SELECT AddGeometryColumn('pred_sites','location',-1,'POINT',2);
CREATE UNIQUE INDEX predSitesByName ON pred_sites (site_name);

CREATE TABLE svn_features
(
    svn_feat_if    SERIAL PRIMARY KEY,
    pred_site_id   int,
    params         varchar(2048),
    score          real,
    generation     int
);

ALTER TABLE sites   ADD CONSTRAINT FK_pred_site_id FOREIGN KEY(pred_site_id) REFERENCES pred_sites (pred_site_id);
ALTER TABLE flights ADD CONSTRAINT FK_pilot_id     FOREIGN KEY(pilot_id)     REFERENCES pilots     (pilot_id)   ON DELETE CASCADE;ALTER TABLE flights ADD CONSTRAINT FK_contest_id   FOREIGN KEY(contest_id)   REFERENCES contests   (contest_id) ON DELETE CASCADE;ALTER TABLE flights ADD CONSTRAINT FK_site_id      FOREIGN KEY(site_id)      REFERENCES sites      (site_id);



INSERT INTO contests (contest_name, website) values ('onlinecontest', 'http://www.onlinecontest.org');INSERT INTO contests (contest_name, website) values ('xcontest',      'http://www.xcontest.org');
INSERT INTO pred_sites (site_name, location, country) values ('Fiesch',      GeomFromText('POINT(46.411933333078998 8.1145333332485592)', -1), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Grindelwald', GeomFromText('POINT(46.656613323997803 8.0552869234381106)', -1), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Niesen',      GeomFromText('POINT(46.644222222434202 7.6476388888888902)', -1), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Rothenflue',  GeomFromText('POINT(47.018337296086003 8.7019931164065607)', -1), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Engelberg',   GeomFromText('POINT(46.840041666560701 8.4053166664971304)', -1), 'CH');
INSERT INTO pred_sites (site_name, location, country) values ('Uetliberg',   GeomFromText('POINT()', -1), 'CH');
CREATE TABLE weather_pred
(
	weather_pred_id SERIAL PRIMARY KEY,
	pred_time   timestamp  without time zone NOT NULL,
	level       real                         NOT NULL,
	parameter   varchar(10)                  NOT NULL,
	value       real                         NOT NULL
);
SELECT AddGeometryColumn('weather_pred','location',-1,'POINT',2);

CREATE INDEX weatherByLvLParamTime ON weather_pred (level, parameter, pred_time);
CREATE INDEX weatherByLocation     ON weather_pred USING GIST ( location );

