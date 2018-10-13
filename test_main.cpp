#include "ip_filter.h"

#ifdef IP_FILTER_BENCH
#  include "timer.h"
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <functional>
#include <algorithm>

using namespace std::string_literals;

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_main)

  // ("",  '.') -> [""]
  BOOST_AUTO_TEST_CASE(test_split_empty_input)
  {
    auto parts = ipv4::split(""s,'.');
    BOOST_CHECK(parts.size() == 1);
    BOOST_CHECK(parts.at(0) == ""s);
  }

  // ("11", '.') -> ["11"]
  BOOST_AUTO_TEST_CASE(test_split_no_delimiters)
  {
    auto parts = ipv4::split("11"s,'.');
    BOOST_CHECK(parts.size() == 1);
    BOOST_CHECK(parts.at(0) == "11"s);
  }

  // ("..", '.') -> ["", "", ""]
  BOOST_AUTO_TEST_CASE(test_split_only_delimiters)
  {
    auto parts = ipv4::split(".."s,'.');
    BOOST_CHECK(parts.size() == 3);
    BOOST_CHECK(parts.at(0).empty());
    BOOST_CHECK(parts.at(1).empty());
    BOOST_CHECK(parts.at(2).empty());
  }

  // ("11.", '.') -> ["11", ""]
  BOOST_AUTO_TEST_CASE(test_split_empty_second)
  {
    auto parts = ipv4::split("11."s,'.');
    BOOST_CHECK(parts.size() == 2);
    BOOST_CHECK(parts.at(0) == "11"s);
    BOOST_CHECK(parts.at(1).empty());
  }

  // (".11", '.') -> ["", "11"]
  BOOST_AUTO_TEST_CASE(test_split_empty_first)
  {
    auto parts = ipv4::split(".11"s,'.');
    BOOST_CHECK(parts.size() == 2);
    BOOST_CHECK(parts.at(0).empty());
    BOOST_CHECK(parts.at(1) == "11"s);
  }

  // ("11.22", '.') -> ["11", "22"]
  BOOST_AUTO_TEST_CASE(test_split_two_parts)
  {
    auto parts = ipv4::split("11.22"s,'.');
    BOOST_CHECK(parts.size() == 2);
    BOOST_CHECK(parts.at(0) == "11"s);
    BOOST_CHECK(parts.at(1) == "22"s);
  }

  BOOST_AUTO_TEST_CASE(test_sorting)
  {
    try {
      std::ifstream data("test_data.tsv");
      BOOST_CHECK(data.is_open());

      auto ip_pool = ipv4::pool_t();

      for(std::string line; std::getline(data, line);)
	ip_pool.push_back(ipv4::split(ipv4::split(line, '\t').at(0), '.'));

      ipv4::sort(ip_pool);

      // 222.173.235.246
      // 222.130.177.64
      // 222.82.198.61
      // ...
      // 1.231.69.33
      // 1.87.203.225
      // 1.70.44.170
      // 1.29.168.152
      // 1.1.234.8

      auto correct_head = ipv4::pool_t({
	    {"222"s,"173"s,"235"s,"246"s}
	  , {"222"s,"130"s,"177"s,"64"s }
	  , {"222"s,"82"s ,"198"s,"61"s }
	  });

      auto correct_tail = ipv4::pool_t({
	    {"1"s,"231"s,"69"s ,"33"s }
	  , {"1"s,"87"s ,"203"s,"225"s}
	  , {"1"s,"70"s ,"44"s,"170"s}
	  , {"1"s,"29"s ,"168"s,"152"s}
	  , {"1"s,"1"s  ,"234"s,"8"s  }
	  });

      BOOST_CHECK(std::equal(std::begin(correct_head), std::end(correct_head), std::begin(ip_pool)));

      auto endIt = std::end(ip_pool);
      auto addrIt = std::find_if(
	  std::begin(ip_pool)
	  , endIt
	  , [byte_str = ipv4::byte_t("1")](const ipv4::addr_t& addr) {return (addr.front() == byte_str);}
	  );
      BOOST_CHECK(addrIt != endIt);
      BOOST_CHECK(std::equal(std::begin(correct_tail), std::end(correct_tail), addrIt, endIt));
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

  BOOST_AUTO_TEST_CASE(test_filter_by_the_first_byte)
  {
    std::ifstream data("test_data.tsv");
    BOOST_CHECK(data.is_open());

    auto ip_pool = ipv4::pool_t();

    for(std::string line; std::getline(data, line);)
      ip_pool.push_back(ipv4::split(ipv4::split(line, '\t').at(0), '.'));

    ipv4::sort(ip_pool);
    auto filtered_pool = ipv4::filter(ip_pool, 1);

    // 1.231.69.33
    // 1.87.203.225
    // 1.70.44.170
    // 1.29.168.152
    // 1.1.234.8
 
    auto correct_pool = ipv4::pool_t({
	  {"1"s,"231"s,"69"s ,"33"s }
	, {"1"s,"87"s ,"203"s,"225"s}
	, {"1"s,"70"s ,"44"s,"170"s }
	, {"1"s,"29"s ,"168"s,"152"s}
	, {"1"s,"1"s  ,"234"s,"8"s  }
	});

    BOOST_CHECK(correct_pool == filtered_pool);
  }

  BOOST_AUTO_TEST_CASE(test_filter_by_two_first_bytes)
  {
    std::ifstream data("test_data.tsv");
    BOOST_CHECK(data.is_open());

    auto ip_pool = ipv4::pool_t();

    for(std::string line; std::getline(data, line);)
      ip_pool.push_back(ipv4::split(ipv4::split(line, '\t').at(0), '.'));

    ipv4::sort(ip_pool);
    auto filtered_pool = ipv4::filter(ip_pool, 46, 70);

    // 46.70.225.39
    // 46.70.147.26
    // 46.70.113.73
    // 46.70.29.76

    auto correct_pool = ipv4::pool_t({
	  {"46"s ,"70"s ,"225"s,"39"s}
	, {"46"s ,"70"s ,"147"s,"26"s}
	, {"46"s ,"70"s ,"113"s,"73"s}
	, {"46"s ,"70"s ,"29"s ,"76"s}
	});

    BOOST_CHECK(correct_pool == filtered_pool);
  }

  BOOST_AUTO_TEST_CASE(test_filter_any)
  {
    std::ifstream data("test_data.tsv");
    BOOST_CHECK(data.is_open());

    auto ip_pool = ipv4::pool_t();

    for(std::string line; std::getline(data, line);)
      ip_pool.push_back(ipv4::split(ipv4::split(line, '\t').at(0), '.'));

    ipv4::sort(ip_pool);
    auto filtered_pool = ipv4::filter_any(ip_pool, 46);

    auto correct_pool = ipv4::pool_t({
	  {"186"s,"204"s,"34"s ,"46"s }
	, {"186"s,"46"s ,"222"s,"194"s}
	, {"185"s,"46"s ,"87"s ,"231"s}
	, {"185"s,"46"s ,"86"s ,"132"s}
	, {"185"s,"46"s ,"86"s ,"131"s}
	, {"185"s,"46"s ,"86"s ,"131"s}
	, {"185"s,"46"s ,"86"s ,"22"s }
	, {"185"s,"46"s ,"85"s ,"204"s}
	, {"185"s,"46"s ,"85"s ,"78"s }
	, {"68"s ,"46"s ,"218"s,"208"s}
	, {"46"s ,"251"s,"197"s,"23"s }
	, {"46"s ,"223"s,"254"s,"56"s }
	, {"46"s ,"223"s,"254"s,"56"s }
	, {"46"s ,"182"s,"19"s ,"219"s}
	, {"46"s ,"161"s,"63"s ,"66"s }
	, {"46"s ,"161"s,"61"s ,"51"s }
	, {"46"s ,"161"s,"60"s ,"92"s }
	, {"46"s ,"161"s,"60"s ,"35"s }
	, {"46"s ,"161"s,"58"s ,"202"s}
	, {"46"s ,"161"s,"56"s ,"241"s}
	, {"46"s ,"161"s,"56"s ,"203"s}
	, {"46"s ,"161"s,"56"s ,"174"s}
	, {"46"s ,"161"s,"56"s ,"106"s}
	, {"46"s ,"161"s,"56"s ,"106"s}
	, {"46"s ,"101"s,"163"s,"119"s}
	, {"46"s ,"101"s,"127"s,"145"s}
	, {"46"s ,"70"s ,"225"s,"39"s }
	, {"46"s ,"70"s ,"147"s,"26"s }
	, {"46"s ,"70"s ,"113"s,"73"s }
	, {"46"s ,"70"s ,"29"s ,"76"s }
	, {"46"s ,"55"s ,"46"s ,"98"s }
	, {"46"s ,"49"s ,"43"s ,"85"s }
	, {"39"s ,"46"s ,"86"s ,"85"s }
	, {"5"s  ,"189"s,"203"s,"46"s }
	});

    BOOST_CHECK(correct_pool == filtered_pool);
  }


#ifdef IP_FILTER_BENCH

  BOOST_AUTO_TEST_CASE(measure_reading_with_push_back)
  {
    const size_t counts = 1000;
    timer execution_timer;
    execution_timer.start();

    for (size_t i = 0; i < counts; i++)
    {
      std::ifstream data("test_data.tsv");
      BOOST_CHECK(data.is_open());

      auto ip_pool = ipv4::pool_t();

      for(std::string line; std::getline(data, line);)
	ip_pool.push_back(ipv4::split(ipv4::split(line, '\t').at(0), '.'));
    }

    double execution_time = execution_timer.stop() / counts;
    std::cout << '\n' << "measure_reading_with_push_back_time: " << std::fixed << std::setprecision(0) << execution_time << '\n';
  }

  BOOST_AUTO_TEST_CASE(measure_reading_with_emplace_back)
  {
    const size_t counts = 1000;
    timer execution_timer;
    execution_timer.start();

    for (size_t i = 0; i < counts; i++)
    {
      std::ifstream data("test_data.tsv");
      BOOST_CHECK(data.is_open());

      auto ip_pool = ipv4::pool_t();

      for(std::string line; std::getline(data, line);)
	ip_pool.emplace_back(ipv4::split(ipv4::split(line, '\t').at(0), '.'));
    }

    double execution_time = execution_timer.stop() / counts;
    std::cout << '\n' << "measure_reading_with_emplace_back_time: " << std::fixed << std::setprecision(0) << execution_time << '\n';
  }

  BOOST_AUTO_TEST_CASE(measure_filter_by_two_first_bytes)
  {
    std::ifstream data("test_data.tsv");
    BOOST_CHECK(data.is_open());

    auto ip_pool = ipv4::pool_t();

    for(std::string line; std::getline(data, line);)
      ip_pool.emplace_back(ipv4::split(ipv4::split(line, '\t').at(0), '.'));

    ipv4::sort(ip_pool);

    const size_t counts = 1000;
    timer execution_timer;
    execution_timer.start();

    for (size_t i = 0; i < counts; i++)
    {
      auto filtered_pool = ipv4::filter(ip_pool, 46, 70);
    }

    double execution_time = execution_timer.stop() / counts;
    std::cout << '\n' << "measure_filter_by_two_first_bytes_time: " << std::fixed << std::setprecision(0) << execution_time << '\n';
  }
#endif // IP_FILTER_BENCH

BOOST_AUTO_TEST_SUITE_END()
