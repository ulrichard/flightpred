// flightpred
#include "geo_parser.h"
// boost
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_attribute.hpp>
#include <boost/spirit/include/phoenix1_binders.hpp>
#include <boost/spirit/include/phoenix1_actor.hpp>
#include <boost/spirit/include/phoenix1_functions.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/phoenix1_statements.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/ref.hpp>
//standard library
#include <stdexcept>
#include <utility>

using namespace flightpred;
using namespace geometry;
using namespace boost::spirit::classic;
using boost::ref;
using namespace phoenix;
using std::pair;
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
// our parser grammar
////////////////////////////////////////////////////////////////////////////
// variable handling

struct setllFunctor
{
	setllFunctor(point_ll_deg &pos, bool isLon) : pos_(pos), isLon_(isLon) {}
//	setllFunctor(const evalVariableFunctor &cpy) : m_varfunc(cpy.m_varfunc) { }


	template<typename T1> struct result				{ typedef void type; };

	void operator()(const double val) const
	{
	    if(isLon_)
            pos_.lon(val);
        else
            pos_.lat(val);
	}

	point_ll_deg &pos_;
	const bool isLon_;
};

phoenix::function<setllFunctor> set_lat(point_ll_deg &pos)
{
	return setllFunctor(pos, false);
}

phoenix::function<setllFunctor> set_lon(point_ll_deg &pos)
{
	return setllFunctor(pos, true);
}

struct dbl_closure : boost::spirit::classic::closure<dbl_closure, double>
{
	member1 val;
};
/*
struct pair_closure : boost::spirit::classic::closure<pair_closure, pair<double, double> >
{
	member1 val;
};
*/
struct position_grammar : public grammar<position_grammar/*, pair_closure::context_t*/>
{
	position_grammar(point_ll_deg &pos)
		: pos_(pos) { }

	template <typename ScannerT>
	struct definition
	{
		definition(position_grammar const& self)
		{

			top = (rLat[set_lat(self.pos_)(arg1)] >> rLon[set_lon(self.pos_)(arg1)])
			    | (rLon[set_lon(self.pos_)(arg1)] >> rLat[set_lat(self.pos_)(arg1)]);

			rLat = rLatD[rLat.val = arg1] | rLatDM[rLat.val = arg1] | rLatDMS[rLat.val = arg1];

			rLon = rLonD[rLon.val = arg1] | rLonDM[rLon.val = arg1] | rLonDMS[rLon.val = arg1];

			rLatD
				=   ureal_p[rLatD.val =  arg1] >> !ch_p('°') >> 'N' | 'N' >> ureal_p[rLatD.val =  arg1] >> !ch_p('°')
				|   ureal_p[rLatD.val = -arg1] >> !ch_p('°') >> 'S' | 'S' >> ureal_p[rLatD.val = -arg1] >> !ch_p('°')
				;

            rLonD
                =   ureal_p[rLonD.val =  arg1] >> !ch_p('°') >> 'E' | 'E' >> ureal_p[rLonD.val =  arg1] >> !ch_p('°')
				|   ureal_p[rLonD.val = -arg1] >> !ch_p('°') >> 'W' | 'W' >> ureal_p[rLonD.val = -arg1] >> !ch_p('°')
				;

            rLatDM
                =   int_p[rLatDM.val =  arg1] >> !ch_p('°') >> ureal_p[rLatDM.val += (arg1 / 60.0)] >> !ch_p('\'') >> 'N'
                |   'N' >> int_p[rLatDM.val = arg1] >> !ch_p('°') >> ureal_p[rLatDM.val += (arg1 / 60.0)] >> !ch_p('\'')
                |   int_p[rLatDM.val = -arg1] >> !ch_p('°') >> ureal_p[rLatDM.val -= (arg1 / 60.0)] >> !ch_p('\'') >> 'S'
                |   'S' >> int_p[rLatDM.val = -arg1] >> !ch_p('°') >> ureal_p[rLatDM.val -= (arg1 / 60.0)] >> !ch_p('\'')
                ;

            rLonDM
                =   int_p[rLonDM.val =  arg1] >> !ch_p('°') >> ureal_p[rLonDM.val += (arg1 / 60.0)] >> !ch_p('\'') >> 'E'
                |   'E' >> int_p[rLonDM.val = arg1] >> !ch_p('°') >> ureal_p[rLonDM.val += (arg1 / 60.0)] >> !ch_p('\'')
                |   int_p[rLonDM.val = -arg1] >> !ch_p('°') >> ureal_p[rLonDM.val -= (arg1 / 60.0)] >> !ch_p('\'') >> 'W'
                |   'W' >> int_p[rLonDM.val = -arg1] >> !ch_p('°') >> ureal_p[rLonDM.val -= (arg1 / 60.0)] >> !ch_p('\'')
                ;

            rLatDMS
                =   int_p[rLatDMS.val =  arg1] >> !ch_p('°') >> int_p[rLatDMS.val += (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLatDMS.val += (arg1 / 3600.0)] >> !ch_p('"') >> 'N'
                |   'N' >> int_p[rLatDMS.val =  arg1] >> !ch_p('°') >> int_p[rLatDMS.val += (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLatDMS.val += (arg1 / 3600.0)] >> !ch_p('"')
                |   int_p[rLatDMS.val = -arg1] >> !ch_p('°') >> int_p[rLatDMS.val -= (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLatDMS.val -= (arg1 / 3600.0)] >> !ch_p('"') >> 'S'
                |   'S' >> int_p[rLatDMS.val = -arg1] >> !ch_p('°') >> int_p[rLatDMS.val -= (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLatDMS.val -= (arg1 / 3600.0)] >> !ch_p('"')
                ;

            rLonDMS
                =   int_p[rLonDMS.val =  arg1] >> !ch_p('°') >> int_p[rLonDMS.val += (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLonDMS.val += (arg1 / 3600.0)] >> !ch_p('"') >> 'E'
                |   'E' >> int_p[rLonDMS.val =  arg1] >> !ch_p('°') >> int_p[rLonDMS.val += (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLonDMS.val += (arg1 / 3600.0)] >> !ch_p('"')
                |   int_p[rLonDMS.val = -arg1] >> !ch_p('°') >> int_p[rLonDMS.val -= (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLonDMS.val -= (arg1 / 3600.0)] >> !ch_p('"') >> 'W'
                |   'W' >> int_p[rLonDMS.val = -arg1] >> !ch_p('°') >> int_p[rLonDMS.val -= (arg1 / 60.0)] >> !ch_p('\'') >> ureal_p[rLonDMS.val -= (arg1 / 3600.0)] >> !ch_p('"')
                ;


		} // definition constructor

        typedef rule<ScannerT, dbl_closure::context_t> dbl_rule;
        dbl_rule rLat, rLon, rLatD, rLonD, rLatDM, rLonDM, rLatDMS, rLonDMS;
		rule<ScannerT> top;

		rule<ScannerT> const& start() const { return top; }
	}; // struct definition

    point_ll_deg &pos_;
//	double &lat_, &lon_;

}; // struct calculator
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
point_ll_deg flightpred::parse_position(const std::string &pos)
{
    if(pos.empty())
		throw std::runtime_error("trying to evaluate empty position");

	point_ll_deg posll;
	position_grammar posgrm(posll);    //  Our parser
	if(!parse(pos.c_str(), posgrm/*[var(lalo) = arg1]*/, space_p).full)
		throw std::runtime_error("failed to parse the position \"" + pos + "\"");
	return posll;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
