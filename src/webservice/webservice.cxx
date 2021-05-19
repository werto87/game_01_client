#include "src/webservice/webservice.hxx"
#include "src/util/util.hxx"
#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>
#include <game_01_shared_class/serialization.hxx>
#include <unistd.h>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace beast = boost::beast;         // from <bo   ost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

Webservice::Webservice (boost::asio::io_context &_io_context, MessagesToSendToServer &messagesToSendToServer, StateMachine &stateMachine) : _messagesToSendToServer{ messagesToSendToServer }, _stateMachine{ stateMachine }, ws{ _io_context } {}

awaitable<void>
Webservice::connect ()
{

  // co_await ws.async_connect ({ tcp::v4 (), 55555 }, use_awaitable);

  // beast::error_code ec;

  // These objects perform our I/O
  tcp::resolver resolver (ws.get_executor ());

  // Look up the domain name
  //   auto const results = resolv  er  .async_resolve (host, port,  yield[ec]);
  auto const results = co_await resolver.async_resolve ({ tcp::v4 (), 55555 }, use_awaitable);
  // if (ec) return fail (ec, "resolve");

  // Set a timeout on the operation
  beast::get_lowest_layer (ws).expires_after (std::chrono::seconds (30));

  // Make the connection on the IP address we get from a lookup
  auto ep = co_await beast::get_lowest_layer (ws).async_connect (results, use_awaitable);
  // if (ec) return fail (ec, "connect");

  // Update the host_ string. This will provide the value of the
  // Host HTTP header during the WebSocket handshake.
  // See https://tools.ietf.org/html/rfc7230#section-5.4
  std::string host = "localhost:" + std::to_string (ep.port ());

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  beast::get_lowest_layer (ws).expires_never ();

  // Set suggested timeout settings for the websocket
  ws.set_option (websocket::stream_base::timeout::suggested (beast::role_type::client));

  // Set a decorator to change the User-Agent of the handshake
  ws.set_option (websocket::stream_base::decorator ([] (websocket::request_type &req) { req.set (http::field::user_agent, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-client-coro"); }));

  // Perform the websocket handshake
  co_await ws.async_handshake (host, "/", boost::asio::use_awaitable);
}

boost::asio::awaitable<void>
Webservice::read ()
{
  try
    {
      for (;;)
        {
          handleMessage (co_await my_read ());
        }
    }
  catch (std::exception &e)
    {
      std::cout << "echo  Exception: " << e.what () << std::endl;
    }
}

awaitable<std::string>
Webservice::my_read ()
{
  // std::cout << "read" << std::endl;
  boost::beast::flat_buffer buffer;
  co_await ws.async_read (buffer, boost::asio::use_awaitable);
  auto msg = boost::beast::buffers_to_string (buffer.data ());
  std::cout << "{Server} " << msg << "'" << std::endl;
  co_return msg;
}

awaitable<void>
Webservice::writeToServer ()
{
  using namespace boost::beast;
  using namespace boost::asio;
  try
    {
      for (;;)
        {
          auto timer = steady_timer (co_await this_coro::executor);
          using namespace std::chrono_literals;
          timer.expires_after (100ms);
          co_await timer.async_wait (use_awaitable);
          while (not _messagesToSendToServer.messagesToSendToServer.empty ())
            {
              auto result = std::string{};
              result = std::move (_messagesToSendToServer.messagesToSendToServer.front ());
              _messagesToSendToServer.messagesToSendToServer.pop_front ();
              co_await ws.async_write (buffer (std::move (result)), use_awaitable);
            }
        }
    }
  catch (std::exception &e)
    {
      std::printf ("echo Exception:  %s\n", e.what ());
    }
}

void
Webservice::closeSocket ()
{
  ws.close ("quit connection");
}

void
Webservice::handleMessage (std::string const &msg)
{
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () == 2)
    {
      auto const &typeToSearch = splitMesssage.at (0);
      auto const &objectAsString = splitMesssage.at (1);
      bool typeFound = false;
      boost::hana::for_each (shared_class::sharedClasses, [&] (const auto &x) {
        if (typeToSearch == confu_soci::typeNameWithOutNamespace (x))
          {
            typeFound = true;
            _stateMachine.process_event (confu_boost::toObject<std::decay_t<decltype (x)>> (objectAsString));
            return;
          }
      });
      if (not typeFound) std::cout << "could not find a match for typeToSearch '" << typeToSearch << "'" << std::endl;
    }
}