#include "src/ui/ui.hxx"
#include <Magnum/Platform/EmscriptenApplication.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <boost/json/src.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/cmdline.hpp>
#include <cxxopts.hpp>
#include <iostream>

namespace
{
Corrade::Containers::Pointer<ImGuiExample> emscriptenApplicationInstance;
}

int
main (int argc, char **argv)
{
  std::cout << "argc: " << argc << std::endl;
  std::copy (argv + 1, argv + argc, std::ostream_iterator<const char *> (std::cout, "\n"));
  cxxopts::Options options ("modern-durak-client", "A brief description");
  // clang-format off
      options.add_options()
        ("w,websocket-url", "url to websocket --websocket-url wss://www.example.com/socketserver", cxxopts::value<std::string>()->default_value("wss://localhost:55555"))
        ("t,touch", "set this option if input type is touch and not keyboard")
        ("h,help", "Print usage")
    ;
  // clang-format on
  auto result = options.parse (argc, argv);
  if (result.count ("help"))
    {
      std::cout << options.help () << std::endl;
      exit (0);
    }
  int tmpArgc = 1;      // work around to ignore commandline input for magnum imgui
  char *tmpArgv[1];     // work around to ignore commandline input for magnum imgui
  tmpArgv[0] = argv[0]; // work around to ignore commandline input for magnum imgui

  auto websocketUrl = result["websocket-url"].as<std::string> ();
  if (websocketUrl == "{{WEBSOCKET-URL}}")
    {
      websocketUrl = "wss://localhost:55555";
      std::cout << "websocketUrl == \"{{WEBSOCKET-URL}}\" using " << websocketUrl << std::endl;
    }
  emscriptenApplicationInstance.reset (new ImGuiExample{ { tmpArgc, tmpArgv }, result["touch"].as<bool> (), websocketUrl });
  emscriptenApplicationInstance->redraw ();
}