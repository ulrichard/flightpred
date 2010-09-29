PROJECT(flightpred_scripts)
SET(CMAKE_MODULE_PATH ${flightpred_scripts_SOURCE_DIR})

INSTALL(FILES create_flightpred_db.sql    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred)
INSTALL(FILES update_flightpred_db.sql    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred)
INSTALL(FILES flightpred_make_forecast.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred)
INSTALL(FILES forecast.php                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/flightpred/www_docroot)

