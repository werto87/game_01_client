#ifndef CA43EC75_CA67_44F7_B24E_0441BC1855FE
#define CA43EC75_CA67_44F7_B24E_0441BC1855FE

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
  static bool hasLoginState ();
  static bool isLoggedIn ();
  static bool hasAccountCreateSuccessState ();
  static bool isAccountCreateSuccess ();
  static void resetSession ();
  static std::string logInMessageFromServer ();
  static bool isCreateAccountError ();
  static std::string createAccountError ();
  static size_t channelCount ();
  static std::vector<std::string> channelNames ();
  static std::vector<std::string> const &messagesForChannel (std::string const &channel);
  static bool hasCreateGameLobbyName ();
  static std::string createGameLobbyName ();
  static std::vector<std::string> const &accountNamesInCreateGameLobby ();
  static size_t getMaxUsersInGameLobby ();
  static std::string getAccountName ();
  static bool hasRelogToError ();
  static std::string relogToError ();
  static void removeRelogToError ();
  static bool hasRelogToDestination ();
  static std::string relogToDestination ();
  static void removeRelogToDestinationMessage ();

  static void removeCreateAccountErrorMessage ();

  static void removeIsAccountCreateSuccess ();

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

#endif /* CA43EC75_CA67_44F7_B24E_0441BC1855FE */
