#include "src/ui/ui.hxx"
#include <Corrade/Utility/Arguments.h>
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
  Corrade::Utility::Arguments args;
  // clang-format off
  args.addSkippedPrefix("magnum", "engine-specific options")
    .addOption('w', "websocket-url", "wss://localhost:55555").setHelp("websocket-url", "url to websocket")
    .addBooleanOption('t', "touch").setHelp("touch", "set this option if input type is touch and not keyboard")
    .setGlobalHelp("A brief description")
    .parse(argc, argv);
  // clang-format on
  auto websocketUrl = args.value ("websocket-url");
  if (websocketUrl == "{{WEBSOCKET-URL}}")
    {
      websocketUrl = "wss://localhost:55555";
      std::cout << "websocketUrl == \"{{WEBSOCKET-URL}}\" using " << websocketUrl << std::endl;
    }
  emscriptenApplicationInstance.reset (new ImGuiExample{ { argc, argv }, args.isSet ("touch"), websocketUrl });
  emscriptenApplicationInstance->redraw ();
}