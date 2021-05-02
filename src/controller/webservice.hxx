#ifndef F06EFE63_F044_49C0_9D23_F73303313BC1
#define F06EFE63_F044_49C0_9D23_F73303313BC1

#include "src/model/session.hxx"
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstddef>
#include <deque>
#include <game_01_shared_class/serialization.hxx>
#include <pipes/pipes.hpp>
#include <string>
#include <vector>
class WebserviceController
{
public:
  static std::vector<std::string> handleMessage (std::string const &msg);
  static void sendMessage (std::string const &msg);
  static void sendMessage (std::vector<std::string> &&msg);
  static bool messageEmpty ();
  static std::string popFront ();
  static bool
  hasLoginState ()
  {
    return session.isLoggedIn.has_value ();
  }
  static bool
  isLoggedIn ()
  {
    return session.isLoggedIn.value ();
  }

  static bool
  hasAccountCreateSuccessState ()
  {
    return session.isAccountCreateSuccess.has_value ();
  }

  static bool
  isAccountCreateSuccess ()
  {
    return session.isAccountCreateSuccess.value ();
  }

  static void
  resetSession ()
  {
    session = Session{};
  }

  static std::string
  logInMessageFromServer ()
  {
    return session.loggInMessageFromServer.value ();
  }

  static bool
  isCreateAccountError ()
  {
    return session.createAccountErrorMessage.has_value ();
  }

  static std::string
  createAccountError ()
  {
    return session.createAccountErrorMessage.value ();
  }

  static size_t
  channelCount ()
  {
    return session.channelMessages.size ();
  }

  static std::vector<std::string>
  channelNames ()
  {
    auto result = std::vector<std::string>{};
    session.channelMessages >>= pipes::transform ([] (auto const &channelAndMessages) { return std::get<0> (channelAndMessages); }) >>= pipes::push_back (result);
    return result;
  }

  static std::vector<std::string> const &
  messagesForChannel (std::string const &channel)
  {
    return session.channelMessages.at (channel);
  }

  static bool
  hasCreateGameLobbyName ()
  {
    return session.gameLobbyName.has_value ();
  }

  static std::string
  createGameLobbyName ()
  {
    return session.gameLobbyName.value ();
  }

  static std::vector<std::string> const &
  accountNamesInCreateGameLobby ()
  {
    return session.accountNamesInGameLobby;
  }
  static size_t
  getMaxUsersInGameLobby ()
  {
    return session.maxUserInGameLobby;
  }

  static std::string
  getAccountName ()
  {
    return session.accountName;
  }

  static bool
  hasRelogToError ()
  {
    return session.relogToError.has_value ();
  }

  static std::string
  relogToError ()
  {
    return session.relogToError.value ();
  }

  static void
  removeRelogToError ()
  {
    session.relogToError = {};
  }

  static bool
  hasRelogToDestination ()
  {
    return session.relogToDestination.has_value ();
  }

  static std::string
  relogToDestination ()
  {
    return session.relogToDestination.value ();
  }

  static void
  removeRelogToDestinationMessage ()
  {
    session.relogToDestination = {};
  }

  template <typename TypeToSend>
  static void
  sendObject (TypeToSend const &typeToSend)
  {
    msgToSend.push_back (confu_soci::typeNameWithOutNamespace (typeToSend) + '|' + confu_boost::toString (typeToSend));
  }

private:
  // BEGIN: handle messages from network---------------------------------------------
  static void createAccountSuccess (std::string const &objectAsString);
  static void createAccountError (std::string const &objectAsString);
  static void loginAccountSuccess (std::string const &objectAsString);
  static void loginAccountError (std::string const &objectAsString);
  static void logoutAccountSuccess (std::string const &objectAsString);
  static void joinChannelSuccess (std::string const &objectAsString);
  static void joinChannelError (std::string const &objectAsString);
  static void message (std::string const &objectAsString);
  static void broadCastMessageSuccess (std::string const &objectAsString);
  static void broadCastMessageError (std::string const &objectAsString);
  static void createGameLobbySuccess (std::string const &objectAsString);
  static void createGameLobbyError (std::string const &objectAsString);
  static void joinGameLobbySuccess (std::string const &objectAsString);
  static void joinGameLobbyError (std::string const &objectAsString);
  static void usersInGameLobby (std::string const &objectAsString);
  static void maxUserSizeInCreateGameLobby (std::string const &objectAsString);
  static void setMaxUserSizeInCreateGameLobbyError (std::string const &objectAsString);
  static void leaveGameLobbySuccess (std::string const &objectAsString);
  static void leaveGameLobbyError (std::string const &objectAsString);
  static void wantToRelog (std::string const &objectAsString);
  static void relogToError (std::string const &objectAsString);
  // END: handle messages from network-----------------------------------------------

  static std::deque<std::string> msgToSend;
  static Session session;
};
#endif /* F06EFE63_F044_49C0_9D23_F73303313BC1 */
