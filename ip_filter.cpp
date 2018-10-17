#include "ip_filter.h"

#include <algorithm>
#include <numeric>
#include <limits>

void ipv4::print(std::ostream& stream, const addr_t& ip_addr)
{
  for (auto ip_byte_it = ip_addr.cbegin(); ip_byte_it != ip_addr.cend(); ++ip_byte_it)
  {
    if (ip_byte_it != ip_addr.cbegin())
      stream << '.';
    stream << std::to_string(*ip_byte_it);
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
    , std::greater<addr_t>()
    );
}

ipv4::pool_t ipv4::filter_any(const pool_t& ip_pool, int byte)
{
  auto filtered_pool = pool_t();

  std::copy_if(
      std::begin(ip_pool)
      , std::end(ip_pool)
      , std::back_inserter(filtered_pool)
      , [byte](const addr_t& addr)
	{
	  return std::any_of(
	      addr.cbegin()
	      , addr.cend()
	      , [byte](const byte_t& addr_byte)
		{
		  return (addr_byte == byte);
		}
	      );
	}
      );

  return filtered_pool;
}

ipv4::pool_t ipv4::filter_any_seq(const pool_t& ip_pool, int byte)
{
  auto filtered_pool = pool_t();

  std::copy_if(
      std::begin(ip_pool)
      , std::end(ip_pool)
      , std::back_inserter(filtered_pool)
      , [byte](const addr_t& addr)
	{
	  return (std::find(addr.cbegin(), addr.cend(), byte) != addr.cend());
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

ipv4::addr_t ipv4::to_addr(const std::vector<std::string>& addr_str)
{
  auto addr = addr_t(addr_str.size(), 0);
  std::transform(
      std::begin(addr_str)
      , std::end(addr_str)
      , std::begin(addr)
      , [](const std::string& byte_str)
	{
	  char * end = nullptr;
	  long byte = std::strtol(byte_str.c_str(), &end, 10);
	  if ((errno == ERANGE)
	    ||(end && *end != '\0')
	    ||(byte < 0)
	    ||(byte > static_cast<long>((std::numeric_limits<ipv4::addr_t::value_type>::max)()))
	    )
	  {
	    byte = 0;
	  }
	  return static_cast<ipv4::addr_t::value_type>(byte);
	}
      );
  return addr;
}

ipv4::addr_t ipv4::to_addr(const std::string& addr_str)
{
  auto addr = addr_t();
  addr.reserve(4);
  addr.push_back(0);
  for (const auto& sym : addr_str)
  {
    if (sym == '.')
    {
      addr.push_back(0);
    }
    else
    {
      addr.back() *= 10;
      addr.back() += static_cast<byte_t>(sym) - 48; // ASCII offset of '0'
    }
  }
  return addr;
}
