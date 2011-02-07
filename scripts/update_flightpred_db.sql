
update weather_models set url_past='nomads.ncdc.noaa.gov/data/gfsanl/', grid_step=1.0 where model_name='GFS';


create or replace function create_pred_ignore_table() returns void as
$$
begin

    if not exists(select * from information_schema.tables where table_name='pred_ignore') then

        CREATE TABLE pred_ignore
		(
			pred_ignore_id SERIAL PRIMARY KEY,
			model_id    int           NOT NULL,
			pred_day    date 		  NOT NULL,
			confirmed   boolean		  default false,
			CONSTRAINT FK_model_id FOREIGN KEY(model_id) REFERENCES weather_models(model_id) ON DELETE CASCADE
		);

		CREATE UNIQUE INDEX ignoreByModelDate ON pred_ignore (pred_ignore_id);

    end if;

end;
$$
language 'plpgsql';

select create_pred_ignore_table();
drop function create_pred_ignore_table();


DROP INDEX IF EXISTS sites_by_pred_site_id;
CREATE INDEX sites_by_pred_site_id ON sites (pred_site_id);
DROP INDEX IF EXISTS flightBySite;
CREATE INDEX flightBySite ON flights (site_id, flight_date);


