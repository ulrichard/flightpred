
CREATE TABLE pilots
(
	pilot_id SERIAL PRIMARY KEY,
	pilot_name   varchar(50) NOT NULL,
	country      varchar(20) NOT NULL
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
	location    point       NOT NULL
);

CREATE TABLE flights
(
	flight_id SERIAL PRIMARY KEY,
	pilot_id     int          NOT NULL,
	contest_id   int          NOT NULL,
	site_id      int          NULL,
	country      varchar(20)  NOT NULL,
	flight_date  date         NOT NULL,
	igc_file     varchar(256) NULL,
	distance     real         NOT NULL,
	score	     int          NOT NULL
);

ALTER TABLE flights ADD CONSTRAINT FK_pilot_id   FOREIGN KEY(pilot_id)   REFERENCES pilots   (pilot_id)   ON DELETE CASCADE;