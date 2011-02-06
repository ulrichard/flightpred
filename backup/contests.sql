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
-- Name: contests_contest_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('contests_contest_id_seq', 2, true);


--
-- Data for Name: contests; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY contests (contest_id, contest_name, website) FROM stdin;
1	onlinecontest	http://www.onlinecontest.org
2	xcontest	http://www.xcontest.org
\.


--
-- PostgreSQL database dump complete
--

