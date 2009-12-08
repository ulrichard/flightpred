// flightpred
#include "Populations.h"
// pqxx
#include <pqxx/pqxx>
// witty
#include <Wt/WComboBox>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WStandardItemModel>
// boost
#include <boost/any.hpp>

using namespace flightpred;
using std::string;
using boost::any;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
Populations::Populations(const string &db_conn_str, Wt::WContainerWidget *parent)
    : Wt::WCompositeWidget(parent), impl_(new Wt::WContainerWidget()), db_conn_str_(db_conn_str), chart_(0)
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
    ShowPopulation();

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void Populations::ShowPopulation()
{
    const string site_name = areas_->currentText().narrow();
    if(chart_)
    {
        impl_->removeWidget(chart_);
        chart_ = 0;
    }

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web population");

    std::stringstream sstr;
    sstr << "SELECT * FROM trained_solutions INNER JOIN pred_sites ON "
         << "trained_solutions.pred_site_id = pred_sites.pred_site_id WHERE "
         << "site_name='" << site_name << "' "
         << "ORDER BY train_time ASC";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no population found");

    Wt::WStandardItemModel *model = new Wt::WStandardItemModel(res.size(), 3);
    model->setHeaderData(0, Wt::Horizontal, any(string("train_sol_id")));
    model->setHeaderData(1, Wt::Horizontal, any(string("train_time")));
    model->setHeaderData(2, Wt::Horizontal, any(string("score")));

    for(size_t i=0; i<res.size(); ++i)
    {
        size_t train_sol_id;
        res[i]["train_sol_id"].to(train_sol_id);
        model->setData(i, 0, any(train_sol_id));
        double train_time;
        res[i]["train_time"].to(train_time);
        model->setData(i, 1, any(train_time));
        double score;
        res[i]["score"].to(score);
        model->setData(i, 2, any(score));
    }

    chart_ = new Wt::Chart::WCartesianChart(impl_);
    chart_->resize(800, 500);
    chart_->setTitle("Population performance");
    chart_->setPlotAreaPadding(200, Wt::Right);
    chart_->setModel(model);
    chart_->setXSeriesColumn(1);
    Wt::Chart::WDataSeries data1(Wt::Chart::WDataSeries(2, Wt::Chart::PointSeries, Wt::Chart::Y1Axis));
    data1.setLegendEnabled(true);
    chart_->addSeries(data1);
    chart_->setLegendEnabled(true);

}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

