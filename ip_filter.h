#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>

namespace ipv4
{

  using byte_t = std::string;
  using addr_t = std::vector<byte_t>;
  using pool_t = std::vector<addr_t>;

  std::vector<std::string> split(const std::string &str, char d);

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
    return (addr.at(N) == std::to_string(byte) && bytesPredicate<N+1>(addr, args...));
  }

  pool_t filter_any(const pool_t& ip_pool, int byte);

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
