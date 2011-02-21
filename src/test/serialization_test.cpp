
// boost
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/version.hpp>
#include <boost/serialization/string.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
// std lib
#include <iostream>
#include <sstream>

using namespace boost::unit_test;
namespace bfs = boost::filesystem;
using std::string;
using std::vector;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
struct MyConfig
{
    MyConfig()
        : dbname_("flightpred_test")
    {
        const bfs::path scriptdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "scripts");
        const bfs::path backupdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "backup");

        std::stringstream sstr;
        sstr << "psql -e -a -c \"DROP DATABASE IF EXISTS " << dbname_ << "\"";
        int ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "createdb -e " << dbname_ << "";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "createlang plpgsql " << dbname_ << "";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "psql -e -a -d " << dbname_ << " -f /usr/share/postgresql/8.4/contrib/postgis-1.5/postgis.sql";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "psql -e -a -d " << dbname_ << " -f /usr/share/postgresql/8.4/contrib/postgis-1.5/spatial_ref_sys.sql";
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;

        sstr.str("");
        sstr << "psql -e -a -d " << dbname_ << " -f " << (scriptdir / "create_flightpred_db.sql").external_file_string();
        ret = system(sstr.str().c_str());
        std::cout << sstr.str() << " returned: " << ret << std::endl;



    }
/*
    ~MyConfig()
    {
        std::stringstrm sstr;
        sstr << "psql -e -a -c \"DROP DATABASE IF EXISTS " << dbname_ << "\"";
        int ret = system(sstr.str().c_str());
        std::cout << "dropping the db returned: " << ret;
    }
*/
private:
    const std::string dbname_;
};


BOOST_GLOBAL_FIXTURE(MyConfig);
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
enum BackupFormat
{
    BAK_TEXT,
    BAK_TEXT_COMP,
    BAK_BIN,
    BAK_BIN_COMP
};

static const string BackupFormatTxt[] =
{
    "BAK_TEXT",
    "BAK_TEXT_COMP",
    "BAK_BIN",
    "BAK_BIN_COMP"
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class ArchiveT>
void do_export(ArchiveT& oa)
{
    string teststr("Hallo");
	oa << teststr;

	int testint(19091977);
	oa << testint;

	size_t testsize(777);
	oa << testsize;

	double testdouble(182.75);
	oa << testdouble;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void write_test_file(const bfs::path &backup_dir, const BackupFormat bfmt)
{
    std::stringstream sstr;
    bfs::create_directories(backup_dir / BackupFormatTxt[bfmt]);

    // operating system
#ifdef  __unix__
    sstr << "linux";
 #ifdef __LP64__
    sstr << "64";
 #else
    sstr << "32";
 #endif

#else
 #ifdef _WIN64
    sstr << "win64";
 #else
  #ifdef __WIN32
    sstr << "win32";
  #else
    sstr << "win";
  #endif
 #endif
#endif

    // compiler version
#ifdef __GNUC__
    sstr << "_gcc-" << __GNUC__ << "-" << __GNUC_MINOR__ << "-" << __GNUC_PATCHLEVEL__;
#endif
#ifdef _MSC_VER
    sstr << "_msvc-" << _MSC_VER;
#endif


    // boost version
    sstr << "_boost-" << BOOST_VERSION;

    bfs::path outfile(backup_dir / BackupFormatTxt[bfmt] / sstr.str());
    bfs::ofstream ofs(outfile, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	BOOST_REQUIRE(ofs.good());

    const uint8_t nbfmt = bfmt;
    ofs << nbfmt;

    if(bfmt == BAK_TEXT)
    {
        boost::archive::text_oarchive oa(ofs);
        do_export(oa);
    }
    else if(bfmt == BAK_TEXT_COMP)
    {
        boost::iostreams::filtering_stream<boost::iostreams::output> out;
        out.push(boost::iostreams::zlib_compressor());
        out.push(ofs);
        boost::archive::text_oarchive oa(out);
        do_export(oa);
    }
    else if(bfmt == BAK_BIN)
    {
        boost::archive::binary_oarchive oa(ofs);
        do_export(oa);
    }
    else if(bfmt == BAK_BIN_COMP)
    {
        boost::iostreams::filtering_stream<boost::iostreams::output> out;
        out.push(boost::iostreams::gzip_compressor());
        out.push(ofs);
        boost::archive::binary_oarchive oa(out);
        do_export(oa);
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
template<class ArchiveT>
bool do_import(ArchiveT& ia)
{
    bool ret = true;
    string teststr;
	ia >> teststr;
	BOOST_CHECK_EQUAL("Hallo", teststr);
	if("Hallo" != teststr)
        ret = false;

	int testint(0);
	ia >> testint;
	BOOST_CHECK_EQUAL(19091977, testint);
	if(19091977 != testint)
        ret = false;

	size_t testsize(0);
	ia >> testsize;
	BOOST_CHECK_EQUAL(777, testsize);
	if(777 != testsize)
        ret = false;

	double testdouble(0.0);
	ia >> testdouble;
	BOOST_CHECK_CLOSE(182.75, testdouble, 1);
    if(fabs(182.75 - testdouble) > 0.001)
        ret = false;

    return ret;
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
bool read_test_file(const bfs::path &infile, const BackupFormat bfmt)
{
    BOOST_REQUIRE(bfs::exists(infile));
    bfs::ifstream ifs(infile, std::ios_base::in | std::ios_base::binary);
	BOOST_REQUIRE(ifs.good());

    uint8_t nbfmt = 0;
    ifs >> nbfmt;

    BOOST_CHECK_EQUAL(bfmt, nbfmt);
    if(bfmt != nbfmt)
        return false;

    if(nbfmt == BAK_TEXT)
    {
        boost::archive::text_iarchive ia(ifs);
        return do_import(ia);
    }
    else if(nbfmt == BAK_TEXT_COMP)
    {
        boost::iostreams::filtering_stream<boost::iostreams::input> in;
        in.push(boost::iostreams::zlib_decompressor());
        in.push(ifs);
        boost::archive::text_iarchive ia(in);
        return do_import(ia);
    }
    else if(nbfmt == BAK_BIN)
    {
        boost::archive::binary_iarchive ia(ifs);
        return do_import(ia);
    }
    else if(nbfmt == BAK_BIN_COMP)
    {
        boost::iostreams::filtering_stream<boost::iostreams::input> in;
        in.push(boost::iostreams::gzip_decompressor());
        in.push(ifs);
        boost::archive::binary_iarchive ia(in);
        return do_import(ia);
    }
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
void check_test_files(const bfs::path& testdir,  const BackupFormat bfmt)
{
    bfs::directory_iterator end_itr; // default construction yields past-the-end
    for(bfs::directory_iterator itr(testdir / BackupFormatTxt[bfmt]); itr != end_itr; ++itr)
        if(!bfs::is_directory(itr->status()))
            if(!read_test_file(itr->path(), bfmt))
                BOOST_CHECK_EQUAL("Error in file", itr->path().leaf());
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(SerializationTEXT)
{
    const BackupFormat fmt = BAK_TEXT;
    const bfs::path testdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "backup/serialization_test");

    write_test_file(testdir, fmt);

    check_test_files(testdir, fmt);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(SerializationTEXT_COMP)
{
    const BackupFormat fmt = BAK_TEXT_COMP;
    const bfs::path testdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "backup/serialization_test");

    write_test_file(testdir, fmt);

    check_test_files(testdir, fmt);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
/*  binary doesn't work between 32 and 64 bits

BOOST_AUTO_TEST_CASE(SerializationBIN)
{
    const BackupFormat fmt = BAK_BIN;
    const bfs::path testdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "backup/serialization_test");

    write_test_file(testdir, fmt);

    check_test_files(testdir, fmt);
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
BOOST_AUTO_TEST_CASE(SerializationBIN_COMP)
{
    const BackupFormat fmt = BAK_BIN_COMP;
    const bfs::path testdir(bfs::path(__FILE__).parent_path().parent_path().parent_path() / "backup/serialization_test");

    write_test_file(testdir, fmt);

    check_test_files(testdir, fmt);
}
*/
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////A
