#ifndef A4CD9228_FA91_4A0A_B5FE_62BD8AF67601
#define A4CD9228_FA91_4A0A_B5FE_62BD8AF67601

#include <boost/optional.hpp>
#include <cstddef>
#include <map>
#include <string>
struct Session
{
  boost::optional<bool> isLoggedIn{};
  boost::optional<bool> isAccountCreateSuccess{};
  std::string accountName{};
  boost::optional<std::string> createAccountErrorMessage{};
  boost::optional<std::string> loggInMessageFromServer{};
  std::map<std::string, std::vector<std::string> > channelMessages{};
  boost::optional<std::string> gameLobbyName{};
  size_t maxUserInGameLobby{};
  std::vector<std::string> accountNamesInGameLobby{};
  boost::optional<std::string> relogToError{};
  boost::optional<std::string> relogToDestination{};
};

#endif /* A4CD9228_FA91_4A0A_B5FE_62BD8AF67601  */
