#ifndef A4CD9228_FA91_4A0A_B5FE_62BD8AF67601
#define A4CD9228_FA91_4A0A_B5FE_62BD8AF67601

#include <boost/optional.hpp>
#include <map>
#include <string>
struct Session
{
  boost::optional<bool> isLoggedIn{};
  boost::optional<std::string> loggInMessageFromServer{};
  std::map<std::string, std::vector<std::string> > channelMessages{};
};

#endif /* A4CD9228_FA91_4A0A_B5FE_62BD8AF67601  */
