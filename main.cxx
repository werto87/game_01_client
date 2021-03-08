#include "src/database/database.hxx"
#include "src/ui/ui.hxx"
#include "src/webservice/webservice.hxx"
#include <boost/bind/bind.hpp>
boost::asio::awaitable<void>
read (boost::asio::io_context &io_context, Webservice &webservice)
{
  co_await webservice.connect ();
  boost::asio::co_spawn (
      io_context, [&] () mutable { return webservice.read (); }, boost::asio::detached);
  boost::asio::co_spawn (
      io_context, [&] () mutable { return webservice.writeToServer (); }, boost::asio::detached);
}

boost::asio::awaitable<void>
ui (ImGuiExample &app)
{
  app.redraw ();
  for (;;)
    {
      app.mainLoopIteration ();
      auto timer = boost::asio::steady_timer (co_await boost::asio::this_coro::executor);
      using namespace std::chrono_literals;
      timer.expires_after (1ms);
      co_await timer.async_wait (boost::asio::use_awaitable); // give other tasks a chance to run
    }
}

int
main (int argc, char **argv)
{
  try
    {
      database::createEmptyDatabase ();
      database::createTables ();
      boost::asio::io_context io_context (1);
      auto webservice = Webservice{ io_context };
      ImGuiExample app{ { argc, argv }, webservice.msgToSend };
      boost::asio::signal_set signals (io_context, SIGINT, SIGTERM);
      signals.async_wait ([&] (auto, auto) { io_context.stop (); });
      boost::asio::co_spawn (io_context, boost::bind (read, std::ref (io_context), std::ref (webservice)), boost::asio::detached);
      boost::asio::co_spawn (io_context, boost::bind (ui, std::ref (app)), boost::asio::detached);
      io_context.run ();
    }
  catch (std::exception &e)
    {
      std::printf ("Exception: %s\n", e.what ());
    }
}