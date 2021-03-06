#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdint.h>

namespace ipv4
{

  using byte_t = uint8_t;
  using addr_t = std::vector<byte_t>;
  using pool_t = std::vector<addr_t>;

  std::vector<std::string> split(const std::string &str, char d);
  addr_t to_addr(const std::vector<std::string> &str); //! converts vector of bytes {"xxx", "xxx", "xxx", "XXX"}
  addr_t to_addr(const std::string& addr_str);	       //! converts address of  "xxx.xxx.xxx.xxx" format

  void print(std::ostream&, const pool_t&);
  void print(std::ostream& stream, const addr_t& ip_addr);

  void sort(pool_t& ip_pool);

  template<size_t N>
  bool bytesPredicate(const addr_t&)
  {
    return true;
  }

  template<size_t N, typename... Args>
  bool bytesPredicate(const addr_t& addr, int byte, Args... args)
  {
    return ((addr.at(N) == byte) && bytesPredicate<N+1>(addr, args...));
  }

  pool_t filter_any(const pool_t& ip_pool, int byte);
  pool_t filter_any_seq(const pool_t& ip_pool, int byte);

  template<typename... Args>
  pool_t filter(const pool_t& ip_pool, Args... args)
  {
    auto filtered_pool = pool_t();
    std::copy_if(
	std::begin(ip_pool)
	, std::end(ip_pool)
	, std::back_inserter(filtered_pool)
	, [args...](const addr_t& addr) {return bytesPredicate<0>(addr, args...);}
	);
    return filtered_pool;
  }
}
