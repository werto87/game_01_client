#include "src/ui/ui.hxx"
#include <boost/json/src.hpp>
#include <boost/lexical_cast.hpp>
#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

namespace
{
Corrade::Containers::Pointer<ImGuiExample> emscriptenApplicationInstance;
}
int
main (int argc, char **argv)
{
#ifdef DEBUG
  std::cout << "DEBUG" << std::endl;
#else
  std::cout << "NO DEBUG" << std::endl;
#endif
  std::cout << argc << std::endl;
  std::cout << argv[1] << std::endl;
  // TODO uncom men  t  if da tabase i  s needed
  // i f  w e react i v a te th is   ca r e :  re l ease  wi ll try to  use the  same database which does not work (start the program 2 times and see what happens)
  // debug  will  c r  e   a    t  e   a   n e  w  d atabase  and keeps the o ld which is a w orkaround for starting the program multiple times in parallel
  // createE mptyD atab a se  ();
  // createTables ( );
  emscriptenApplicationInstance.reset (new ImGuiExample{ { argc, argv }, (argv[1] == std::string{ "touch" }) });
  emscriptenApplicationInstance->redraw ();
}