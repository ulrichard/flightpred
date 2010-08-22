// witty
#include <Wt/WContainerWidget>
#include <Wt/WCompositeWidget>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace Wt
{
    class WComboBox;
    class WText;
    class WTable;
    namespace Chart
    {
        class WCartesianChart;
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
namespace flightpred
{
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
class Populations : public Wt::WCompositeWidget
{
public:
    Populations(const std::string &db_conn_str, Wt::WContainerWidget *parent = 0);
    virtual ~Populations() { }

private:
    void ShowPopulation();

    Wt::WContainerWidget *impl_;
    const std::string db_conn_str_;
    Wt::WComboBox *areas_;
    Wt::WComboBox *criteria_;
    Wt::Chart::WCartesianChart *chart_;
    Wt::WText *footertext_;
    Wt::WTable *algo_legend_;
};
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
} // namespace flightpred
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
