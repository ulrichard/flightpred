#ifndef SOLUTION_MANAGER_H_INCLUDED
#define SOLUTION_MANAGER_H_INCLUDED

// flightpred
#include <common/solution_config.h>
//boost
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
// std lib
#include <string>
#include <map>
#include <set>
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class solution_manager : public boost::noncopyable
{
public:
    solution_manager(const std::string &site_name)
        : site_name_(site_name), pred_site_id_(get_pred_site_id(site_name)), weather_(true),
          date_extremes_(weather_.get_feature_date_period(false)), ignored_days_(get_ignored_days(true)) { }
    virtual ~solution_manager() { };

    void evolve_population(const size_t iterations, const double mutation_rate);

    std::auto_ptr<solution_config> load_best_solution(const bool onlyFullyTrained, const double maxTrainSec = 50.0);

    enum BackupFormat
    {
        BAK_TEXT,
        BAK_TEXT_COMP,
        BAK_BIN,
        BAK_BIN_COMP
    };
    void export_solution(const boost::filesystem::path &backup_dir, const BackupFormat bfmt);
    void import_solution(const boost::filesystem::path &backup_dir);

    static const std::set<boost::gregorian::date> get_ignored_days(bool onlyConfirmed);

private:
    static size_t get_pred_site_id(const std::string &site_name);
    std::vector<boost::shared_ptr<solution_config> > initialize_population();
    std::vector<boost::shared_ptr<solution_config> > get_initial_generation();
    void  fill_label_cache();
    const std::vector<double> & get_sample(const std::set<features_weather::feat_desc> &fdesc, const boost::gregorian::date &day);
    bool   used_for_training(const boost::gregorian::date &day) const;
    bool   used_for_validation(const boost::gregorian::date &day) const;
    double test_fitness(const solution_config &sol);
    template<class ArchiveT>
    void  do_export(ArchiveT& oa);
    template<class ArchiveT>
    void  do_import(ArchiveT& ia);


    const std::string site_name_;
    const size_t      pred_site_id_;
    features_weather  weather_;
    const boost::gregorian::date_period date_extremes_;
    const std::set<boost::gregorian::date> ignored_days_;
    typedef std::map<const std::set<features_weather::feat_desc>, std::map<boost::gregorian::date, std::vector<double> > > WeatherFeatureCacheType;
    WeatherFeatureCacheType  weathercache_;
    std::map<boost::gregorian::date, double> max_distances_;
    std::vector<double> training_labels_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
#endif // SOLUTION_MANAGER_H_INCLUDED
