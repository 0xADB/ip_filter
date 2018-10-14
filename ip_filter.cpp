#include "ip_filter.h"

#include <algorithm>
#include <numeric>

void ipv4::print(std::ostream& stream, const addr_t& ip_addr)
{
  for (auto ip_byte_it = ip_addr.cbegin(); ip_byte_it != ip_addr.cend(); ++ip_byte_it)
  {
    if (ip_byte_it != ip_addr.cbegin())
      stream << '.';
    stream << *ip_byte_it;
  }
}

void ipv4::print(std::ostream& stream, const pool_t& ip_pool)
{
  for (const auto& ip_addr : ip_pool)
  {
    print(stream, ip_addr);
    stream << '\n';
  }
}

void ipv4::sort(pool_t& ip_pool)
{
  std::sort(
    std::begin(ip_pool)
    , std::end(ip_pool)
    , [](const ipv4::addr_t& lhs, const ipv4::addr_t& rhs)
      {
	return !std::lexicographical_compare( // couldn't figure out how to apply the std::not2, sorry
	    lhs.cbegin()
	    , lhs.cend()
	    , rhs.cbegin()
	    , rhs.cend()
	    , [](const ipv4::byte_t& lhs, const ipv4::byte_t& rhs)
	      {
		return (
		  (lhs.size() < rhs.size())
		  || (lhs.size() == rhs.size()
		    && std::lexicographical_compare(
			lhs.cbegin()
			, lhs.cend()
			, rhs.cbegin()
			, rhs.cend()
			)
		    )
		  );
	      }
	    );
      }
    );
}

ipv4::pool_t ipv4::filter_any(const pool_t& ip_pool, int byte)
{
  auto filtered_pool = pool_t();

  std::copy_if(
      std::begin(ip_pool)
      , std::end(ip_pool)
      , std::back_inserter(filtered_pool)
      , [byte_str = std::to_string(byte)](const addr_t& addr)
	{
	  return (std::find(addr.cbegin(), addr.cend(), byte_str) != addr.cend());
	}
      );

  return filtered_pool;
}

std::vector<std::string> ipv4::split(const std::string &str, char d)
{
  std::vector<std::string> r;

  std::string::size_type start = 0;
  std::string::size_type stop = str.find_first_of(d);
  while(stop != std::string::npos)
  {
    r.emplace_back(str.substr(start, stop - start));

    start = stop + 1;
    stop = str.find_first_of(d, start);
  }

  r.emplace_back(str.substr(start));

  return r;
}
