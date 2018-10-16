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

      for(std::string line; !data.eof();)
      {
	data >> line;
	data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	ip_pool.emplace_back(ipv4::to_addr(line));
      }

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
	    {222,173,235,246}
	  , {222,130,177,64 }
	  , {222,82 ,198,61 }
	  });

      auto correct_tail = ipv4::pool_t({
	    {1,231,69 ,33 }
	  , {1,87 ,203,225}
	  , {1,70 ,44,170}
	  , {1,29 ,168,152}
	  , {1,1  ,234,8  }
	  });

      BOOST_CHECK(std::equal(std::begin(correct_head), std::end(correct_head), std::begin(ip_pool)));

      auto endIt = std::end(ip_pool);
      auto addrIt = std::find_if(
	  std::begin(ip_pool)
	  , endIt
	  , [byte = 1](const ipv4::addr_t& addr) {return (addr.front() == byte);}
	  );
      BOOST_CHECK(addrIt != endIt);
      BOOST_CHECK(std::equal(std::begin(correct_tail), std::end(correct_tail), addrIt, endIt));
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

  BOOST_AUTO_TEST_CASE(test_sorting_with_to_addr_with_strings)
  {
    try {
      std::ifstream data("test_data.tsv");
      BOOST_CHECK(data.is_open());

      auto ip_pool = ipv4::pool_t();

      for(std::string line; !data.eof();)
      {
	data >> line;
	data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	ip_pool.emplace_back(ipv4::to_addr(line));
      }

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
	    {222,173,235,246}
	  , {222,130,177,64 }
	  , {222,82 ,198,61 }
	  });

      auto correct_tail = ipv4::pool_t({
	    {1,231,69 ,33 }
	  , {1,87 ,203,225}
	  , {1,70 ,44,170}
	  , {1,29 ,168,152}
	  , {1,1  ,234,8  }
	  });

      BOOST_CHECK(std::equal(std::begin(correct_head), std::end(correct_head), std::begin(ip_pool)));

      auto endIt = std::end(ip_pool);
      auto addrIt = std::find_if(
	  std::begin(ip_pool)
	  , endIt
	  , [byte = 1](const ipv4::addr_t& addr) {return (addr.front() == byte);}
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

    for(std::string line; !data.eof();)
    {
      data >> line;
      data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ip_pool.emplace_back(ipv4::to_addr(line));
    }

    ipv4::sort(ip_pool);
    auto filtered_pool = ipv4::filter(ip_pool, 1);

    // 1.231.69.33
    // 1.87.203.225
    // 1.70.44.170
    // 1.29.168.152
    // 1.1.234.8
 
    auto correct_pool = ipv4::pool_t({
	  {1,231,69 ,33 }
	, {1,87 ,203,225}
	, {1,70 ,44,170 }
	, {1,29 ,168,152}
	, {1,1  ,234,8  }
	});

    BOOST_CHECK(correct_pool == filtered_pool);
  }

  BOOST_AUTO_TEST_CASE(test_filter_by_two_first_bytes)
  {
    std::ifstream data("test_data.tsv");
    BOOST_CHECK(data.is_open());

    auto ip_pool = ipv4::pool_t();

    for(std::string line; !data.eof();)
    {
      data >> line;
      data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ip_pool.emplace_back(ipv4::to_addr(line));
    }
    ipv4::sort(ip_pool);
    auto filtered_pool = ipv4::filter(ip_pool, 46, 70);

    // 46.70.225.39
    // 46.70.147.26
    // 46.70.113.73
    // 46.70.29.76

    auto correct_pool = ipv4::pool_t({
	  {46 ,70 ,225,39}
	, {46 ,70 ,147,26}
	, {46 ,70 ,113,73}
	, {46 ,70 ,29 ,76}
	});

    BOOST_CHECK(correct_pool == filtered_pool);
  }

  BOOST_AUTO_TEST_CASE(test_filter_any)
  {
    std::ifstream data("test_data.tsv");
    BOOST_CHECK(data.is_open());

    auto ip_pool = ipv4::pool_t();

    for(std::string line; !data.eof();)
    {
      data >> line;
      data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ip_pool.emplace_back(ipv4::to_addr(line));
    }

    ipv4::sort(ip_pool);
    auto filtered_pool = ipv4::filter_any(ip_pool, 46);

    auto correct_pool = ipv4::pool_t({
	  {186,204,34 ,46 }
	, {186,46 ,222,194}
	, {185,46 ,87 ,231}
	, {185,46 ,86 ,132}
	, {185,46 ,86 ,131}
	, {185,46 ,86 ,131}
	, {185,46 ,86 ,22 }
	, {185,46 ,85 ,204}
	, {185,46 ,85 ,78 }
	, {68 ,46 ,218,208}
	, {46 ,251,197,23 }
	, {46 ,223,254,56 }
	, {46 ,223,254,56 }
	, {46 ,182,19 ,219}
	, {46 ,161,63 ,66 }
	, {46 ,161,61 ,51 }
	, {46 ,161,60 ,92 }
	, {46 ,161,60 ,35 }
	, {46 ,161,58 ,202}
	, {46 ,161,56 ,241}
	, {46 ,161,56 ,203}
	, {46 ,161,56 ,174}
	, {46 ,161,56 ,106}
	, {46 ,161,56 ,106}
	, {46 ,101,163,119}
	, {46 ,101,127,145}
	, {46 ,70 ,225,39 }
	, {46 ,70 ,147,26 }
	, {46 ,70 ,113,73 }
	, {46 ,70 ,29 ,76 }
	, {46 ,55 ,46 ,98 }
	, {46 ,49 ,43 ,85 }
	, {39 ,46 ,86 ,85 }
	, {5  ,189,203,46 }
	});

    BOOST_CHECK(correct_pool == filtered_pool);
  }


#ifdef IP_FILTER_BENCH

  BOOST_AUTO_TEST_CASE(measure_sorting)
  {
    try {
      std::ifstream data("test_data.tsv");
      BOOST_CHECK(data.is_open());

      auto ip_pool = ipv4::pool_t();

      for(std::string line; !data.eof();)
      {
	data >> line;
	data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	ip_pool.emplace_back(ipv4::to_addr(line));
      }

      const size_t counts = 1000;
      timer execution_timer;
      execution_timer.start();

      for (size_t i = 0; i < counts; i++)
      {
	auto sorted_pool = ip_pool;
	ipv4::sort(sorted_pool);
      }

      double execution_time = execution_timer.stop() / counts;
      std::cout << '\n' << std::setw(50) << "measure_sorting_time: " << std::setw(10) << std::fixed << std::setprecision(0) << execution_time << " ns\n";
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

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
	ip_pool.push_back(ipv4::to_addr(ipv4::split(ipv4::split(line, '\t').at(0), '.')));
    }

    double execution_time = execution_timer.stop() / counts;
    std::cout << '\n' << std::setw(50) << "measure_reading_with_push_back_time: " << std::setw(10) << std::fixed << std::setprecision(0) << execution_time << " ns\n";
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
	ip_pool.emplace_back(ipv4::to_addr(ipv4::split(ipv4::split(line, '\t').at(0), '.')));
    }

    double execution_time = execution_timer.stop() / counts;
    std::cout << '\n' << std::setw(50) << "measure_reading_with_emplace_back_time: " << std::setw(10) << std::fixed << std::setprecision(0) << execution_time << " ns\n";
  }

  BOOST_AUTO_TEST_CASE(measure_reading_with_to_addr_with_strings)
  {
    const size_t counts = 1000;
    timer execution_timer;
    execution_timer.start();

    for (size_t i = 0; i < counts; i++)
    {
      std::ifstream data("test_data.tsv");
      BOOST_CHECK(data.is_open());

      auto ip_pool = ipv4::pool_t();

      for(std::string line; !data.eof();)
      {
	data >> line;
	data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	ip_pool.emplace_back(ipv4::to_addr(line));
      }
    }

    double execution_time = execution_timer.stop() / counts;
    std::cout << '\n' << std::setw(50) << "measure_reading_to_addr_with_strings_time: " << std::setw(10) << std::fixed << std::setprecision(0) << execution_time << " ns\n";
  }

  BOOST_AUTO_TEST_CASE(measure_filter_by_two_first_bytes)
  {
    std::ifstream data("test_data.tsv");
    BOOST_CHECK(data.is_open());

    auto ip_pool = ipv4::pool_t();

    for(std::string line; !data.eof();)
    {
      data >> line;
      data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ip_pool.emplace_back(ipv4::to_addr(line));
    }

    ipv4::sort(ip_pool);

    const size_t counts = 1000;
    timer execution_timer;
    execution_timer.start();

    for (size_t i = 0; i < counts; i++)
    {
      auto filtered_pool = ipv4::filter(ip_pool, 46, 70);
    }

    double execution_time = execution_timer.stop() / counts;
    std::cout << '\n' << std::setw(50) << "measure_filter_by_two_first_bytes_time: " << std::setw(10) << std::fixed << std::setprecision(0) << execution_time << " ns\n";
  }
#endif // IP_FILTER_BENCH

BOOST_AUTO_TEST_SUITE_END()
