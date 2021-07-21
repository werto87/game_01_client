#ifndef F78F741D_25FD_4094_A575_7ADBC02883C3
#define F78F741D_25FD_4094_A575_7ADBC02883C3

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/mpl/range_c.hpp>
#include <exception>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
auto
vector_to_string (const T &container, const std::string &delimiter) -> std::string
{
  auto result = std::string{};
  std::for_each (std::begin (container), std::end (container), [&result, &delimiter] (auto const &element) {
    result.append (std::to_string (element));
    result.append (delimiter);
  });
  if (!result.empty ())
    {
      result.pop_back ();
    }
  return result;
}

std::string from_u8string (const std::u8string &s);

template <class T> concept IsEnum = std::is_enum_v<T>;

#endif /* F78F741D_25FD_4094_A575_7ADBC02883C3 */
