#include "src/ui/ui.hxx"
#include <boost/json/src.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/cmdline.hpp>
#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif
#include <argparse/argparse.hpp>
#include <iostream>

namespace
{
Corrade::Containers::Pointer<ImGuiExample> emscriptenApplicationInstance;
}
int
main (int argc, char **argv)
{
  std::cout << "argc: " << argc << std::endl;
  std::cout << "argv[0]: " << argv[0] << std::endl;
  std::cout << "argv[1]: " << argv[1] << std::endl;
  argparse::ArgumentParser program ("program name");
  program.add_argument ("").default_value (false).implicit_value (true); // work around to ignore empty strings
  program.add_argument ("--websocket-url").help (R"(url to websocket ## example usage: --websocket-url "wss://www.example.com/socketserver" ## default value: "wss://localhost:55555")").default_value (std::string ("wss://localhost:55555"));
  program.add_argument ("--touch").help ("change input mode from keyboard to touch").default_value (false).implicit_value (true);
  try
    {
      program.parse_args (argc, argv);
    }
  catch (const std::runtime_error &err)
    {
      std::cout << err.what () << std::endl;
      std::cout << program;
      exit (0);
    }
  emscriptenApplicationInstance.reset (new ImGuiExample{ { argc, argv }, program.get<bool> ("--touch"), program.get<std::string> ("--websocket-url") });
  emscriptenApplicationInstance->redraw ();
}