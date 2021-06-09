#ifndef FF3724DC_98E2_4011_A2D6_6D4050D70CED
#define FF3724DC_98E2_4011_A2D6_6D4050D70CED
#include "src/controller/sendObject.hxx"
#include "src/controller/stateMachine.hxx"
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/beast.hpp>
#include <boost/hana.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>
#include <chrono>
#include <confu_json/concept.hxx>
#include <confu_json/to_json.hxx>
#include <confu_json/to_object.hxx>
#include <deque>
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <functional>
#include <game_01_shared_class/serialization.hxx>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

class Webservice
{

public:
  Webservice (StateMachine &stateMachine) : _stateMachine{ stateMachine } {}

  void
  send (std::string const &messageToSend) const
  {
    EMSCRIPTEN_RESULT result;
    std::cout << "send: '" << messageToSend << "'" << std::endl;
    result = emscripten_websocket_send_utf8_text (ws.value (), messageToSend.c_str ());
    if (result)
      {
        printf ("Failed to emscripten_websocket_send_utf8_text(): %d\n", result);
      }
  }

  boost::asio::awaitable<void>
  writeToServer (std::deque<std::string> &_messagesToSendToServer)
  {
    try
      {
        for (;;)
          {
            auto timer = boost::asio::steady_timer (co_await boost::asio::this_coro::executor);
            using namespace std::chrono_literals;
            timer.expires_after (100ms);
            co_await timer.async_wait (boost::asio::use_awaitable);
            while (ws && not _messagesToSendToServer.empty ())
              {
                auto result = std::string{};
                result = std::move (_messagesToSendToServer.front ());
                _messagesToSendToServer.pop_front ();
                send (result);
              }
          }
      }
    catch (std::exception &e)
      {
        std::printf ("echo Exception:  %s\n", e.what ());
      }
  }

  void
  handleMessage (std::string const &msg)
  {
    std::vector<std::string> splitMesssage{};
    boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
    if (splitMesssage.size () == 2)
      {
        auto const &typeToSearch = splitMesssage.at (0);
        auto const &objectAsString = splitMesssage.at (1);
        bool typeFound = false;
        auto objectAsStringStream = std::stringstream{};
        objectAsStringStream << objectAsString;
        boost::hana::for_each (shared_class::sharedClasses, [&] (const auto &x) {
          if (typeToSearch == typeNameWithOutNamespace (x))
            {
              typeFound = true;
              boost::json::error_code ec{};
              _stateMachine.process_event (confu_json::to_object<std::decay_t<decltype (x)>> (confu_json::read_json (objectAsStringStream, ec)));
              if (ec) std::cout << "ec.message () " << ec.message () << std::endl;
              return;
            }
        });
        if (not typeFound) std::cout << "could not find a match for typeToSearch '" << typeToSearch << "'" << std::endl;
      }
  }
  StateMachine &_stateMachine;
  std::optional<EMSCRIPTEN_WEBSOCKET_T> ws;
};

EM_BOOL onopen (int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData);
EM_BOOL onerror (int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData);
EM_BOOL onclose (int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData);
EM_BOOL onmessage (int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData);

#endif /* FF3724DC_98E2_4011_A2D6_6D4050D70CED */
