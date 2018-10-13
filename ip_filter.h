#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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
  pool_t filter(pool_t&& ip_pool)
  {
    return ip_pool;
  }

  template<size_t N, typename... Args>
  pool_t filter(pool_t&& ip_pool, int byte, Args... args)
  {
    static_assert((N < 4), "number of an IPv4 address bytes to filter by is exceeded (please, use up to 4)");
    ip_pool.erase(
	std::remove_if(
	  std::begin(ip_pool)
	  , std::end(ip_pool)
	  , [byte_str = std::to_string(byte)](const addr_t& addr)
	    {
	      return (addr.at(N) != byte_str);
	    }
	  )
	, std::end(ip_pool)
	);

    return filter<N+1>(std::move(ip_pool), args...);
  }

  template<typename... Args>
  pool_t filter(pool_t ip_pool, Args... args)
  {
    return filter<0>(std::move(ip_pool), args...);
  }

  pool_t filter_any(pool_t ip_pool, int byte);

}
