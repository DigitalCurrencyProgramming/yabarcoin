#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-YBR");
    BOOST_CHECK(GetBoolArg("-YBR"));
    BOOST_CHECK(GetBoolArg("-YBR", false));
    BOOST_CHECK(GetBoolArg("-YBR", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BARo"));
    BOOST_CHECK(!GetBoolArg("-BARo", false));
    BOOST_CHECK(GetBoolArg("-BARo", true));

    ResetArgs("-YBR=0");
    BOOST_CHECK(!GetBoolArg("-YBR"));
    BOOST_CHECK(!GetBoolArg("-YBR", false));
    BOOST_CHECK(!GetBoolArg("-YBR", true));

    ResetArgs("-YBR=1");
    BOOST_CHECK(GetBoolArg("-YBR"));
    BOOST_CHECK(GetBoolArg("-YBR", false));
    BOOST_CHECK(GetBoolArg("-YBR", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-YBR"));
    BOOST_CHECK(!GetBoolArg("-YBR", false));
    BOOST_CHECK(!GetBoolArg("-YBR", true));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-YBR"));
    BOOST_CHECK(!GetBoolArg("-YBR", false));
    BOOST_CHECK(!GetBoolArg("-YBR", true));

    ResetArgs("-YBR -noBAR");  // -YBR should win
    BOOST_CHECK(GetBoolArg("-YBR"));
    BOOST_CHECK(GetBoolArg("-YBR", false));
    BOOST_CHECK(GetBoolArg("-YBR", true));

    ResetArgs("-YBR=1 -noBAR=1");  // -YBR should win
    BOOST_CHECK(GetBoolArg("-YBR"));
    BOOST_CHECK(GetBoolArg("-YBR", false));
    BOOST_CHECK(GetBoolArg("-YBR", true));

    ResetArgs("-YBR=0 -noBAR=0");  // -YBR should win
    BOOST_CHECK(!GetBoolArg("-YBR"));
    BOOST_CHECK(!GetBoolArg("-YBR", false));
    BOOST_CHECK(!GetBoolArg("-YBR", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--YBR=1");
    BOOST_CHECK(GetBoolArg("-YBR"));
    BOOST_CHECK(GetBoolArg("-YBR", false));
    BOOST_CHECK(GetBoolArg("-YBR", true));

    ResetArgs("--noBAR=1");
    BOOST_CHECK(!GetBoolArg("-YBR"));
    BOOST_CHECK(!GetBoolArg("-YBR", false));
    BOOST_CHECK(!GetBoolArg("-YBR", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-YBR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-YBR", "eleven"), "eleven");

    ResetArgs("-YBR -ybr");
    BOOST_CHECK_EQUAL(GetArg("-YBR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-YBR", "eleven"), "");

    ResetArgs("-YBR=");
    BOOST_CHECK_EQUAL(GetArg("-YBR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-YBR", "eleven"), "");

    ResetArgs("-YBR=11");
    BOOST_CHECK_EQUAL(GetArg("-YBR", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-YBR", "eleven"), "11");

    ResetArgs("-YBR=eleven");
    BOOST_CHECK_EQUAL(GetArg("-YBR", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-YBR", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-YBR", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-YBR", 0), 0);

    ResetArgs("-YBR -ybr");
    BOOST_CHECK_EQUAL(GetArg("-YBR", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-ybr", 11), 0);

    ResetArgs("-YBR=11 -ybr=12");
    BOOST_CHECK_EQUAL(GetArg("-YBR", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-ybr", 11), 12);

    ResetArgs("-YBR=NaN -ybr=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-YBR", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-ybr", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--YBR");
    BOOST_CHECK_EQUAL(GetBoolArg("-YBR"), true);

    ResetArgs("--YBR=verbose --ybr=1");
    BOOST_CHECK_EQUAL(GetArg("-YBR", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-ybr", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-YBR"));
    BOOST_CHECK(!GetBoolArg("-YBR", true));
    BOOST_CHECK(!GetBoolArg("-YBR", false));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-YBR"));
    BOOST_CHECK(!GetBoolArg("-YBR", true));
    BOOST_CHECK(!GetBoolArg("-YBR", false));

    ResetArgs("-noBAR=0");
    BOOST_CHECK(GetBoolArg("-YBR"));
    BOOST_CHECK(GetBoolArg("-YBR", true));
    BOOST_CHECK(GetBoolArg("-YBR", false));

    ResetArgs("-YBR --noBAR");
    BOOST_CHECK(GetBoolArg("-YBR"));

    ResetArgs("-noBAR -YBR"); // YBR always wins:
    BOOST_CHECK(GetBoolArg("-YBR"));
}

BOOST_AUTO_TEST_SUITE_END()
