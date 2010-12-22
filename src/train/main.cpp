
// flightpred
#include "area_mgr.h"
#include "common/train_svm.h"
#include "common/grab_grib.h"
#include "common/grab_flights.h"
#include "common/forecast.h"
#include "common/geo_parser.h"
#include "common/solution_manager.h"
#include "common/flightpred_globals.h"
#include "common/reporter.h"
// ggl (boost sandbox)
#include <boost/geometry/extensions/gis/latlong/latlong.hpp>
// boost
#include <boost/program_options.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
// standard library
#include <iostream>
#include <string>

using namespace flightpred;
using namespace flightpred::reporting;
namespace po    = boost::program_options;
namespace bgreg = boost::gregorian;
namespace bfs   = boost::filesystem;
using boost::lexical_cast;
using boost::shared_ptr;
using std::string;
using std::vector;
using std::cout;
using std::endl;


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
vector<string> get_site_names()
{
    vector<string> sites;
    pqxx::transaction<> trans(flightpred_db::get_conn(), "get site names");

    std::stringstream sstr;
    sstr << "SELECT site_name FROM pred_sites";
    pqxx::result res = trans.exec(sstr.str());
    for(size_t i=0; i<res.size(); ++i)
    {
        string tmpstr;
        res[i][0].to(tmpstr);
        sites.push_back(tmpstr);
    }

    return sites;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
int main(int argc, char* argv[])
{
    try
    {
        const string processowner(getenv("USER") ? getenv("USER") : "root");
        string name, position, start_date, end_date, pred_model, backup_fmt, backup_directory(getenv("HOME") ? getenv("HOME") : "/tmp");
        double area_radius = 5000.0, mutation_rate, max_eval_time = 50.0;
        size_t download_pack = 100, iterations;
        string db_host, db_name, db_user, db_password;
        size_t db_port;

        bgreg::date dtend(bgreg::day_clock::local_day());
        bgreg::date dtstart(dtend - bgreg::months(5));
        start_date = bgreg::to_iso_extended_string(dtstart);
        end_date   = bgreg::to_iso_extended_string(dtend);

        ReportDispatcher::inst().add(new ListenerCout(), reporting::DEBUGING);
        bfs::path error_file(bfs::initial_path() / "error.log");
        ReportDispatcher::inst().add(new ListenerFile(error_file), reporting::ERROR);

        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help",		       "produce help message")
            ("get-flights",	       "get flights from online competition (name) [import from local files at the moment]")
            ("register-area",      "register a flight area for prediction (name, position, area-radius[km])")
            ("get-weather",        "get past weather prediction grib data around the registered sites (start-date, end-date, download-pack)")
            ("evolve-population",  "evolve a population of solutions (name, generations, mutation-rate)")
            ("train",		       "train the system with the best performing solution found so far (name, start-date, end-date, max_eval_time)")
            ("export-solution",    "save the best solution for a flying site to a file for backup or transfer (name, backup-dir, backup-fmt)")
            ("import-solution",    "restore solutions that were trained before and saved to a file (name, backup-dir)")
            ("get-future-weather", "get future weather prediction grib data for central europe (download-pack)")
            ("forecast",           "predict possible flights for the next few days")
            ("db-maintenance",     "delete old unused data, analyze indices...")
            ("ignore-errors",      "flag to tell the program not to stop on errors, but instead report and continue.")
            ("name",	      po::value<string>(&name)->default_value(""), "name of the area or competition")
            ("position",      po::value<string>(&position)->default_value("N 0 E 0"), "geographic position")
            ("area-radius",   po::value<double>(&area_radius)->default_value(area_radius), "radius around the flight area to include flights for training [m]")
            ("pred-model",    po::value<string>(&pred_model)->default_value("GFS"), "name of the numeric weather prediction model")
            ("start-date",    po::value<string>(&start_date)->default_value(start_date), "start date (yyyy/mm/dd)")
            ("end-date",      po::value<string>(&end_date)->default_value(end_date),     "end date (yyyy/mm/dd)")
            ("download-pack", po::value<size_t>(&download_pack)->default_value(download_pack), "how many grib messages to download at once")
            ("generations",   po::value<size_t>(&iterations)->default_value(50),          "how many generations to run an evolution")
            ("mutation-rate", po::value<double>(&mutation_rate)->default_value(0.125),    "the probability of mutations in the breeding of the evolution")
            ("max-eval-time", po::value<double>(&max_eval_time)->default_value(max_eval_time), "use only solution with a maximum training time of x[sec] during evolution")
            ("backup-dir",    po::value<string>(&backup_directory)->default_value(backup_directory), "the directory with the saved solutions")
            ("backup-fmt",    po::value<string>(&backup_fmt)->default_value("BAK_TEXT"),  "the format of the backup (BAK_TEXT BAK_TEXT_COMP BAK_BIN BAK_BIN_COMP)")
            ("db-host",       po::value<string>(&db_host)->default_value("localhost"),    "name or ip of the database server")
            ("db-port",       po::value<size_t>(&db_port)->default_value(5432),           "port of the database server")
            ("db-name",       po::value<string>(&db_name)->default_value("flightpred"),   "name of the database")
            ("db-user",       po::value<string>(&db_user)->default_value(processowner),   "name of the database user")
            ("db-password",   po::value<string>(&db_password)->default_value(""),         "password of the database user")
            ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        dtstart = bgreg::from_string(start_date);
        dtend   = bgreg::from_string(end_date);
        const string db_conn_str = "host=" + db_host + " port=" + lexical_cast<string>(db_port)
                                 + " dbname=" + db_name + " user=" + db_user
                                 + (db_password.empty() ? string("") : " password=" + db_password);
        try
        {
            flightpred_db::init(db_conn_str);
        }
        catch(std::exception &ex)
        {
            try
            {
                const string db_conn_str = "host=" + db_host + " port=" + lexical_cast<string>(db_port)
                                     + " dbname=" + db_name + " user=flightpred password=flightpred";

                flightpred_db::init(db_conn_str);
            }
            catch(std::exception &ex0)
            {
                try
                {
                    if(db_password != "")
                        throw;

                    cout << "Database password for user " << db_user << ":";
                    std::cin >> db_password;

                    const string db_conn_str = "host=" + db_host + " port=" + lexical_cast<string>(db_port)
                                     + " dbname=" + db_name + " user=" + db_user + " password=" + db_password;

                    flightpred_db::init(db_conn_str);
                }
                catch(std::exception &ex)
                {
                    std::stringstream sstr;
                    sstr << desc << std::endl << std::endl
                         << "Failed to connect to the database " << db_conn_str << std::endl
                         << "For further help see : https://help.ubuntu.com/community/PostgreSQL" << std::endl
                         << "Error: " << ex.what();
                    throw std::runtime_error(sstr.str());
                }
            }
        }
        const boost::geometry::model::ll::point<> pos = parse_position(position);

        const vector<string> site_names = get_site_names();

        bool show_help_msg = true;

        if(vm.count("get-flights"))
        {
            flight_grabber gr(flight_grabber::XCONTEST);
            gr.grab_flights(dtstart, dtend);
            show_help_msg = false;
        }

        if(vm.count("register-area"))
        {
            area_mgr am;
            am.add_area(name, pos, area_radius);
            show_help_msg = false;
        }

        if(vm.count("get-weather"))
        {
            if(pred_model == "GFS")
            {
                grib_grabber_gfs_past gr(download_pack, vm.count("ignore-errors"));
                gr.grab_grib(dtstart, dtend);
            }
            else
                throw std::invalid_argument("unknown model: " + pred_model);
            show_help_msg = false;
        }

        if(vm.count("evolve-population"))
        {
            if(name.length())
            {
                solution_manager mgr(name);
                mgr.evolve_population(iterations, mutation_rate);
            }
            else
                BOOST_FOREACH(const string &site_name, site_names)
                {
                    solution_manager mgr(site_name);
                    mgr.evolve_population(iterations, mutation_rate);
                }

            show_help_msg = false;
        }

        if(vm.count("train"))
        {
            train_svm trainer;
            if(name.length())
                trainer.train(name, dtstart, dtend, max_eval_time);
            else
                std::for_each(site_names.begin(), site_names.end(),
                    boost::bind(&train_svm::train, boost::ref(trainer), _1, dtstart, dtend, max_eval_time));

            show_help_msg = false;
        }

        if(vm.count("export-solution"))
        {
            bfs::path backup_dir(backup_directory);

            solution_manager::BackupFormat bfmt = solution_manager::BAK_TEXT;
            if(backup_fmt == "BAK_TEXT_COMP")
                bfmt = solution_manager::BAK_TEXT_COMP;
            else if(backup_fmt == "BAK_BIN")
                bfmt = solution_manager::BAK_BIN;
            else if(backup_fmt == "BAK_BIN_COMP")
                bfmt = solution_manager::BAK_BIN_COMP;

            if(name.length())
            {
                solution_manager mgr(name);
                mgr.export_solution(backup_dir, bfmt);
            }
            else
                BOOST_FOREACH(const string &site_name, site_names)
                {
                    solution_manager mgr(site_name);
                    mgr.export_solution(backup_dir, bfmt);
                }
            show_help_msg = false;
        }

        if(vm.count("import-solution"))
        {
            bfs::path backup_dir(backup_directory);

            if(name.length())
            {
                solution_manager mgr(name);
                mgr.import_solution(backup_dir);
            }
            else
                BOOST_FOREACH(const string &site_name, site_names)
                {
                    solution_manager mgr(site_name);
                    mgr.import_solution(backup_dir);
                }
            show_help_msg = false;
        }

        if(vm.count("get-future-weather"))
        {
            if(pred_model == "GFS")
            {
                grib_grabber_gfs_future gr(download_pack);
                gr.grab_grib(boost::posix_time::hours(24 * 4));
            }
            else if(pred_model == "GFS_NDAP")
            {
                grib_grabber_gfs_OPeNDAP gr;
                gr.grab_grib(boost::posix_time::hours(24 * 4));
            }
            else
                throw std::invalid_argument("unknown model: " + pred_model);
            show_help_msg = false;
        }

        if(vm.count("forecast"))
        {
            const size_t pred_days = 3;
            forecast fcst;

            if(name.length())
                fcst.prediction_run(name, pred_days);
            else
                std::for_each(site_names.begin(), site_names.end(),
                    boost::bind(&forecast::prediction_run, boost::ref(fcst), _1, pred_days));

            show_help_msg = false;
        }

        if(vm.count("db-maintenance"))
        {
            {
                pqxx::transaction<> trans(flightpred_db::get_conn(), "db-maintenance");

                report(INFO) << "Delete old records with future weather predictions that are no longer in the future.";
                std::stringstream sstr;
                sstr << "delete from weather_pred_future where pred_time < '"
                     << bgreg::to_iso_extended_string(bgreg::day_clock::local_day() - bgreg::days(2)) << " 00:00:00' ";
                trans.exec(sstr.str());
                trans.commit();
            }

            {
                pqxx::nontransaction nontrans(flightpred_db::get_conn());
                report(INFO) << "prune deleted records and update index analysis.";
                nontrans.exec("VACUUM ANALYZE");
                nontrans.commit();
            }

            show_help_msg = false;
        }

        if(vm.count("help") || show_help_msg)
        {
            report(WARN) << desc;
            return 1;
        }
    }
    catch(std::exception &ex)
    {
        report(ERROR) << "critical error : " << ex.what();
        return 1;
    }

    return 0;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

