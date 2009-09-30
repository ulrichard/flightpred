
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
	site_id SERIAL PRIMARY KEY,
	site_name   varchar(50) NOT NULL,
	location    point       NOT NULL,
	country     varchar(5)  NOT NULL
);

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

ALTER TABLE flights ADD CONSTRAINT FK_pilot_id   FOREIGN KEY(pilot_id)   REFERENCES pilots   (pilot_id)   ON DELETE CASCADE;ALTER TABLE flights ADD CONSTRAINT FK_contest_id FOREIGN KEY(contest_id) REFERENCES contests (contest_id) ON DELETE CASCADE;ALTER TABLE flights ADD CONSTRAINT FK_site_id    FOREIGN KEY(site_id)    REFERENCES sites    (site_id);

INSERT INTO contests (contest_name, website) values ('onlinecontest', 'http://www.onlinecontest.org');INSERT INTO contests (contest_name, website) values ('xcontest',      'http://www.xcontest.org');
