--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- Name: weather_models_model_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('weather_models_model_id_seq', 3, true);


--
-- Data for Name: weather_models; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY weather_models (model_id, model_name, grid_step, url_past, url_future) FROM stdin;
2	GFS_NDAP	1		nomads.ncep.noaa.gov:9090/dods/gfs/
1	GFS	1	nomads.ncdc.noaa.gov/data/gfsanl/	nomads.ncep.noaa.gov/pub/data/nccf/com/gfs/prod/
\.


--
-- PostgreSQL database dump complete
--

