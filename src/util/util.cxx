#include "src/util/util.hxx"
std::string
from_u8string (const std::u8string &s)
{
  return std::string (s.begin (), s.end ());
}