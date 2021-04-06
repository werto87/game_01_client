#ifndef C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#define C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#include "src/util/util.hxx"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <deque>
#include <sys/types.h>
class Webservice
{
public:
  Webservice (boost::asio::io_context &_io_context);

  boost::asio::awaitable<void> connect ();

  boost::asio::awaitable<void> sendMsg (std::string msg);

  boost::asio::awaitable<void> read ();

  boost::asio::awaitable<void> writeToServer ();

  std::shared_ptr<std::deque<std::string> > msgToSend = std::make_shared<std::deque<std::string> > ();

private:
  boost::asio::awaitable<std::string> my_read ();

  boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
};
#endif /* C5B152C0_B968_4BBC_B1F9_5823AA8FAD87 */
