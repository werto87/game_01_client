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
          auto msgs = handleMessage (co_await my_read ());
          _messagesToSendToServer.messagesToSendToServer.insert (_messagesToSendToServer.messagesToSendToServer.end (), std::make_move_iterator (msgs.begin ()), std::make_move_iterator (msgs.end ()));
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
Webservice::createAccountSuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::CreateAccountSuccess> (objectAsString));
}

void
Webservice::createAccountError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::CreateAccountError> (objectAsString));
}

void
Webservice::loginAccountSuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::LoginAccountSuccess> (objectAsString));
}

void
Webservice::loginAccountError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::LoginAccountError> (objectAsString));
}

void
Webservice::logoutAccountSuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::LogoutAccountSuccess> (objectAsString));
}

void
Webservice::joinChannelSuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::JoinChannelSuccess> (objectAsString));
}

void
Webservice::joinChannelError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::JoinChannelError> (objectAsString));
}

void
Webservice::message (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::Message> (objectAsString));
}

void
Webservice::broadCastMessageSuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::BroadCastMessageSuccess> (objectAsString));
}

void
Webservice::broadCastMessageError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::BroadCastMessageError> (objectAsString));
}

void
Webservice::createGameLobbySuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::CreateGameLobbySuccess> (objectAsString));
}

void
Webservice::createGameLobbyError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::CreateGameLobbyError> (objectAsString));
}

void
Webservice::joinGameLobbySuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::JoinGameLobbySuccess> (objectAsString));
}

void
Webservice::joinGameLobbyError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::JoinGameLobbyError> (objectAsString));
}

void
Webservice::usersInGameLobby (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::UsersInGameLobby> (objectAsString));
}

void
Webservice::maxUserSizeInCreateGameLobby (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::MaxUserSizeInCreateGameLobby> (objectAsString));
}

void
Webservice::setMaxUserSizeInCreateGameLobbyError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::SetMaxUserSizeInCreateGameLobbyError> (objectAsString));
}

void
Webservice::leaveGameLobbySuccess (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::LeaveGameLobbySuccess> (objectAsString));
}

void
Webservice::leaveGameLobbyError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::LeaveGameLobbyError> (objectAsString));
}

void
Webservice::wantToRelog (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::WantToRelog> (objectAsString));
}

void
Webservice::relogToError (std::string const &objectAsString)
{
  _stateMachine.process_event (confu_boost::toObject<shared_class::RelogToError> (objectAsString));
}

std::vector<std::string>
Webservice::handleMessage (std::string const &msg)
{
  auto result = std::vector<std::string>{};
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () == 2)
    {
      auto typeToSearch = splitMesssage.at (0);
      auto objectAsString = splitMesssage.at (1);
      if (typeToSearch == "CreateAccountSuccess")
        {
          createAccountSuccess (objectAsString);
        }
      else if (typeToSearch == "CreateAccountError")
        {
          createAccountError (objectAsString);
        }
      else if (typeToSearch == "LoginAccountSuccess")
        {
          loginAccountSuccess (objectAsString);
        }
      else if (typeToSearch == "LoginAccountError")
        {
          loginAccountError (objectAsString);
        }
      else if (typeToSearch == "LogoutAccountSuccess")
        {
          logoutAccountSuccess (objectAsString);
        }
      else if (typeToSearch == "JoinChannelSuccess")
        {
          joinChannelSuccess (objectAsString);
        }
      else if (typeToSearch == "JoinChannelError")
        {
          joinChannelError (objectAsString);
        }
      else if (typeToSearch == "Message")
        {
          message (objectAsString);
        }
      else if (typeToSearch == "BroadCastMessageSuccess")
        {
          broadCastMessageSuccess (objectAsString);
        }
      else if (typeToSearch == "BroadCastMessageError")
        {
          broadCastMessageError (objectAsString);
        }
      else if (typeToSearch == "CreateGameLobbySuccess")
        {
          createGameLobbySuccess (objectAsString);
        }
      else if (typeToSearch == "CreateGameLobbyError")
        {
          createGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "JoinGameLobbySuccess")
        {
          joinGameLobbySuccess (objectAsString);
        }
      else if (typeToSearch == "JoinGameLobbyError")
        {
          joinGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "UsersInGameLobby")
        {
          usersInGameLobby (objectAsString);
        }
      else if (typeToSearch == "MaxUserSizeInCreateGameLobby")
        {
          maxUserSizeInCreateGameLobby (objectAsString);
        }
      else if (typeToSearch == "SetMaxUserSizeInCreateGameLobbyError")
        {
          setMaxUserSizeInCreateGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "LeaveGameLobbySuccess")
        {
          leaveGameLobbySuccess (objectAsString);
        }
      else if (typeToSearch == "LeaveGameLobbyError")
        {
          leaveGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "WantToRelog")
        {
          wantToRelog (objectAsString);
        }
      else
        {
          std::cout << "could not find a match for typeToSearch '" << typeToSearch << "'" << std::endl;
        }
    }
  return result;
}