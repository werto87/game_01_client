#include "src/ui/ui.hxx"
#include <boost/json/src.hpp>
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
  // TODO uncomment if database is needed
  // if we reactivate this care: release will try to use the same database which does not work (start the program 2 times and see what happens)
  // debug will create a new database and keeps the old which is a workaround for starting the program multiple times in parallel
  // createEmptyDatabase ();
  // createTables ();
  emscriptenApplicationInstance.reset (new ImGuiExample{ { argc, argv } });
  emscriptenApplicationInstance->redraw ();
}