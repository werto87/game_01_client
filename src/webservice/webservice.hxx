#ifndef C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#define C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#include "src/controller/stateMachine.hxx"
#include "src/util/util.hxx"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <deque>
#include <sys/types.h>
class Webservice
{
public:
  Webservice (boost::asio::io_context &_io_context, MessagesToSendToServer &messagesToSendToServer, StateMachine &stateMachine);

  boost::asio::awaitable<void> connect ();

  boost::asio::awaitable<void> read ();

  boost::asio::awaitable<void> writeToServer ();

  void closeSocket ();

private:
  boost::asio::awaitable<std::string> my_read ();
  MessagesToSendToServer &_messagesToSendToServer;
  StateMachine &_stateMachine;
  boost::beast::websocket::stream<boost::beast::tcp_stream> ws;

  // BEGIN: handle messages from network---------------------------------------------
  std::vector<std::string> handleMessage (std::string const &msg);
  void createAccountSuccess (std::string const &objectAsString);
  void createAccountError (std::string const &objectAsString);
  void loginAccountSuccess (std::string const &objectAsString);
  void loginAccountError (std::string const &objectAsString);
  void logoutAccountSuccess (std::string const &objectAsString);
  void joinChannelSuccess (std::string const &objectAsString);
  void joinChannelError (std::string const &objectAsString);
  void message (std::string const &objectAsString);
  void broadCastMessageSuccess (std::string const &objectAsString);
  void broadCastMessageError (std::string const &objectAsString);
  void createGameLobbySuccess (std::string const &objectAsString);
  void createGameLobbyError (std::string const &objectAsString);
  void joinGameLobbySuccess (std::string const &objectAsString);
  void joinGameLobbyError (std::string const &objectAsString);
  void usersInGameLobby (std::string const &objectAsString);
  void maxUserSizeInCreateGameLobby (std::string const &objectAsString);
  void setMaxUserSizeInCreateGameLobbyError (std::string const &objectAsString);
  void leaveGameLobbySuccess (std::string const &objectAsString);
  void leaveGameLobbyError (std::string const &objectAsString);
  void wantToRelog (std::string const &objectAsString);
  void relogToError (std::string const &objectAsString);
  void relogToSuccess (std::string const &objectAsString);
  void startGame (std::string const &objectAsString);
  void gameData (std::string const &objectAsString);
  void durakAttackSuccess (std::string const &objectAsString);
  void durakAttackError (std::string const &objectAsString);
  void durakDefendSuccess (std::string const &objectAsString);
  void durakDefendError (std::string const &objectAsString);
  // END: handle messages from network-----------------------------------------------
};
#endif /* C5B152C0_B968_4BBC_B1F9_5823AA8FAD87 */
