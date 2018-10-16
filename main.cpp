#include "ip_filter.h"

#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char const *argv[])
{
  try
  {
    std::ios::sync_with_stdio(false);

    auto ip_pool = ipv4::pool_t();;

    for(std::string line; !std::cin.eof();)
    {
      std::cin >> line;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ip_pool.emplace_back(ipv4::to_addr(line));
    }

    ipv4::sort(ip_pool);
    ipv4::print(std::cout, ip_pool);

    auto filtered_pool = ipv4::filter(ip_pool, 1);
    ipv4::print(std::cout, filtered_pool);

    filtered_pool = ipv4::filter(ip_pool, 46, 70);
    ipv4::print(std::cout, filtered_pool);

    filtered_pool = ipv4::filter_any(ip_pool, 46);
    ipv4::print(std::cout, filtered_pool);

  }
  catch(const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
