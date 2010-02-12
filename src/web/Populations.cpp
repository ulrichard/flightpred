// flightpred
#include "Populations.h"
// pqxx
#include <pqxx/pqxx>
// witty
#include <Wt/WComboBox>
#include <Wt/WText>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WStandardItemModel>
// boost
#include <boost/any.hpp>
#include <boost/regex.hpp>
// std lib
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <iostream>

using namespace flightpred;
using boost::any;
using std::string;
using std::vector;
using std::set;
using std::map;
using std::pair;
using std::make_pair;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
Populations::Populations(const string &db_conn_str, Wt::WContainerWidget *parent)
    : Wt::WCompositeWidget(parent), impl_(new Wt::WContainerWidget()), db_conn_str_(db_conn_str), chart_(0), footertext_(0)
{
    setImplementation(impl_);

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web populations");

    std::stringstream sstr;
    sstr << "SELECT site_name from pred_sites";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no sites found");

    areas_ = new  Wt::WComboBox(impl_);
    for(size_t i=0; i<res.size(); ++i)
    {
        string site_name;
        res[i]["site_name"].to(site_name);
        areas_->addItem(site_name);
    }
    areas_->setCurrentIndex(0);
#if WT_SERIES >= 0x3
    areas_->sactivated().connect(SLOT(this, Populations::ShowPopulation));
#else
    areas_->sactivated.connect(SLOT(this, Populations::ShowPopulation));
#endif
    ShowPopulation(areas_->currentText());

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void Populations::ShowPopulation(const Wt::WString &site_name)
{
    if(chart_)
    {
        impl_->removeWidget(chart_);
        chart_ = 0;
    }
    if(footertext_)
    {
        impl_->removeWidget(footertext_);
        footertext_ = 0;
    }

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web population");

    std::stringstream sstr;
    sstr << "SELECT * FROM trained_solutions INNER JOIN pred_sites ON "
         << "trained_solutions.pred_site_id = pred_sites.pred_site_id WHERE "
         << "site_name='" << site_name.narrow() << "' "
         << "ORDER BY train_time ASC";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no population found");

    boost::regex regx("\\w+\\(\\w+");
    set<string> confclasses;
    typedef map<size_t, vector<pair<string, double> > > SolutionsT;
    SolutionsT  solutions;
    double max_train_time = 0.0;
    static const double train_time_treshold = 60.0;

    for(size_t i=0; i<res.size(); ++i)
    {
        size_t train_sol_id;
        res[i]["train_sol_id"].to(train_sol_id);
        string config;
        res[i]["configuration"].to(config);
        boost::smatch regxmatch;
        if(boost::regex_search(config, regxmatch, regx))
        {
            const string confclass = regxmatch[0] + ")";
            double train_time;
            res[i]["train_time"].to(train_time);
            double validation_error;
            res[i]["validation_error"].to(validation_error);

            confclasses.insert(confclass);
            if(train_time <= train_time_treshold)
                solutions[train_time * 100].push_back(make_pair(confclass, validation_error));
            max_train_time = std::max(max_train_time, train_time);
        }
    }

    Wt::WStandardItemModel *model = new Wt::WStandardItemModel(res.size(), 1 + confclasses.size());
    model->setHeaderData(0, Wt::Horizontal, any(string("train_time")));
    for(set<string>::iterator it = confclasses.begin(); it != confclasses.end(); ++it)
        model->setHeaderData(1 + std::distance(confclasses.begin(), it), Wt::Horizontal, any(*it));

    for(SolutionsT::iterator it = solutions.begin(); it != solutions.end(); ++it)
    {
        const size_t modrow = std::distance(solutions.begin(), it);
        model->setData(modrow, 0, any(it->first / 100.0));
        for(vector<pair<string, double> >::iterator itv = it->second.begin(); itv != it->second.end(); ++itv)
        {
            const size_t modcol = 1 + std::distance(confclasses.begin(), confclasses.find(itv->first));
            model->setData(modrow, modcol, any(itv->second));
        }
    }


    chart_ = new Wt::Chart::WCartesianChart(impl_);
    chart_->setType(Wt::Chart::ScatterPlot);
    chart_->resize(800, 500);
    chart_->setTitle("Population performance");
    chart_->setPlotAreaPadding(200, Wt::Right);
    chart_->setPlotAreaPadding(30,  Wt::Bottom);
    chart_->setModel(model);
    chart_->setXSeriesColumn(0);
    chart_->setLegendEnabled(true);
    chart_->axis(Wt::Chart::XAxis).setScale(Wt::Chart::LinearScale);
//    chart_->axis(Wt::Chart::XAxis).setMaximum(60.0);
    chart_->axis(Wt::Chart::XAxis).setLabelFormat("%.2f");
    chart_->axis(Wt::Chart::XAxis).setTitle("training_time");
    chart_->axis(Wt::Chart::YAxis).setTitle("error");
    for(set<string>::iterator it = confclasses.begin(); it != confclasses.end(); ++it)
    {
        Wt::Chart::WDataSeries data1(Wt::Chart::WDataSeries(1 + std::distance(confclasses.begin(), it), Wt::Chart::PointSeries, Wt::Chart::Y1Axis));
        data1.setLegendEnabled(true);
        chart_->addSeries(data1);
    }


    sstr.str("");
    sstr << "Longest training time : " << max_train_time;

    footertext_ = new Wt::WText(sstr.str(), impl_);
    std::cout << sstr.str() << std::endl;

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

