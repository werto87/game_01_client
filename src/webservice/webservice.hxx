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
  // TODO use stateMachine send message to send to server and not webservice controller message
  boost::asio::awaitable<std::string> my_read ();
  MessagesToSendToServer &_messagesToSendToServer;
  StateMachine &_stateMachine;
  boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
};
#endif /* C5B152C0_B968_4BBC_B1F9_5823AA8FAD87 */
