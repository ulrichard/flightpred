// flightpred
#include "Populations.h"
// pqxx
#include <pqxx/pqxx>
// witty
#include <Wt/WComboBox>
#include <Wt/WText>
#include <Wt/WColor>
#include <Wt/WBrush>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WStandardItemModel>
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WTable>
// boost
#include <boost/any.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
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
    : Wt::WCompositeWidget(parent), impl_(new Wt::WContainerWidget()), db_conn_str_(db_conn_str), chart_(0), footertext_(0), algo_legend_(0)
{
    setImplementation(impl_);

    pqxx::connection conn(db_conn_str_);
    pqxx::transaction<> trans(conn, "web populations");
    std::stringstream sstr;

    // areas
    Wt::WText *txtarea = new Wt::WText("Area", impl_);
    areas_ = new  Wt::WComboBox(impl_);
    sstr << "SELECT site_name from pred_sites";
    pqxx::result res = trans.exec(sstr.str());
    if(!res.size())
        throw std::runtime_error("no sites found");
    for(size_t i=0; i<res.size(); ++i)
    {
        string site_name;
        res[i]["site_name"].to(site_name);
        areas_->addItem(site_name);
    }
    areas_->setCurrentIndex(0);
    areas_->activated().connect(SLOT(this, Populations::ShowPopulation));


    // criteria
    Wt::WText *txtcrit = new Wt::WText("Criteria", impl_);
    criteria_ = new  Wt::WComboBox(impl_);
    criteria_->addItem("Algorithm");
    criteria_->addItem("Generation");
    criteria_->setCurrentIndex(0);
    criteria_->activated().connect(SLOT(this, Populations::ShowPopulation));



    ShowPopulation();
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void Populations::ShowPopulation()
{
    try
    {
        const Wt::WString &site_name = areas_->currentText();
        Wt::WApplication::instance()->log("debug") <<  "Populations::ShowPopulation() for " << site_name;

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
        if(algo_legend_)
        {
            impl_->removeWidget(algo_legend_);
            algo_legend_ = 0;
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
        {
            std::cout << "critical error during session initialization : " << std::endl;
            footertext_ = new Wt::WText("no population found", impl_);
            return;
        }

        boost::regex regx("\\w+\\(\\w+");
        set<string> confclasses;
        typedef map<size_t, vector<pair<string, double> > > SolutionsT; // train_time, algorithm, validation_error
        SolutionsT  solutions;
        double max_train_time = 0.0;
        static const double train_time_treshold = 60.0;

        for(size_t i=0; i<res.size(); ++i)
        {
            size_t train_sol_id;
            res[i]["train_sol_id"].to(train_sol_id);
            size_t generation;
            res[i]["generation"].to(generation);
            string config;
            res[i]["configuration"].to(config);
            boost::smatch regxmatch;
            if(boost::regex_search(config, regxmatch, regx))
            {
                string confclass = regxmatch[0] + ")";
                double train_time;
                res[i]["train_time"].to(train_time);
                double validation_error;
                res[i]["validation_error"].to(validation_error);

                if(criteria_->currentText() == "Generation")
                    confclass = (boost::format("%03d") % generation).str();

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
            const size_t npos = std::distance(confclasses.begin(), it);
            Wt::Chart::WDataSeries data1(Wt::Chart::WDataSeries(1 + npos, Wt::Chart::PointSeries, Wt::Chart::Y1Axis));
            data1.setLegendEnabled(true);

            if(criteria_->currentText() == "Generation")
            {
                // make older generations fade
                const float brightness = 1.0 - (static_cast<float>(npos) / confclasses.size()); // 0.0 to 1.0
                data1.setBrush(Wt::WBrush(Wt::WColor(255, brightness * 255, brightness * 255)));
            }
            chart_->addSeries(data1);
        }

        sstr.str("");
        sstr << "Longest training time : " << max_train_time;
        footertext_ = new Wt::WText(sstr.str(), impl_);
        std::cout << sstr.str() << std::endl;

        if(criteria_->currentText() == "Algorithm")
        {
            Wt::WTable *algo_legend_ = new Wt::WTable(impl_);
            new Wt::WText("<b>Algorithm descriptions:</b>", Wt::XHTMLText, algo_legend_->elementAt(0, 0));
            new Wt::WText("<b>DLIB_KRLS:</b>", Wt::XHTMLText, algo_legend_->elementAt(1, 0));
            new Wt::WText("<a href='http://dclib.sourceforge.net/ml.html#krls'>kernel recursive least squares</a>"
                        " -> <a href='http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.2.9587&rep=rep1&type=pdf'>detailed paper</a>", Wt::XHTMLText, algo_legend_->elementAt(1, 1));
            new Wt::WText("<b>DLIB_RVM:</b>", Wt::XHTMLText, algo_legend_->elementAt(2, 0));
            new Wt::WText("<a href='http://dclib.sourceforge.net/ml.html#rvm_regression_trainer'>relevance vector machine</a>", Wt::XHTMLText, algo_legend_->elementAt(2, 1));
            new Wt::WText("<b>Kernel descriptions:</b>", Wt::XHTMLText, algo_legend_->elementAt(3, 0));
            new Wt::WText("<b>RBF:</b>", Wt::XHTMLText, algo_legend_->elementAt(4, 0));
            new Wt::WText("<a href='http://dclib.sourceforge.net/ml.html#radial_basis_kernel'>radial basis function</a>", Wt::XHTMLText, algo_legend_->elementAt(4, 1));
            new Wt::WText("<b>SIG:</b>", Wt::XHTMLText, algo_legend_->elementAt(5, 0));
            new Wt::WText("<a href='http://dclib.sourceforge.net/ml.html#sigmoid_kernel'>sigmoid</a>", Wt::XHTMLText, algo_legend_->elementAt(5, 1));
            new Wt::WText("<b>POLY:</b>", Wt::XHTMLText, algo_legend_->elementAt(6, 0));
            new Wt::WText("<a href='http://dclib.sourceforge.net/ml.html#polynomial_kernel'>polynomial</a>", Wt::XHTMLText, algo_legend_->elementAt(6, 1));

        }
    }
    catch(std::exception& ex)
    {
        std::cout << "Exception in Populations::ShowPopulation() : " << ex.what() << std::endl;
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A

