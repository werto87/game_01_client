#ifndef A76A25AE_A804_47C0_8549_6F15C0EB7035
#define A76A25AE_A804_47C0_8549_6F15C0EB7035

#include <Magnum/Platform/Sdl2Application.h>

#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

#include <Magnum/ImGuiIntegration/Context.hpp>

#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

namespace Magnum
{
namespace Examples
{
class ImGuiExample : public Platform::Application
{

public:
  explicit ImGuiExample (const Arguments &arguments);

  void drawEvent () override;

  void viewportEvent (ViewportEvent &event) override;

  void keyPressEvent (KeyEvent &event) override;
  void keyReleaseEvent (KeyEvent &event) override;

  void mousePressEvent (MouseEvent &event) override;
  void mouseReleaseEvent (MouseEvent &event) override;
  void mouseMoveEvent (MouseMoveEvent &event) override;
  void mouseScrollEvent (MouseScrollEvent &event) override;
  void textInputEvent (TextInputEvent &event) override;

  ImGuiIntegration::Context _imgui{ NoCreate };

private:
  bool _showDemoWindow = true;
  Float _fontSize = 0.5f;
};

}
}

#endif /* A76A25AE_A804_47C0_8549_6F15C0EB7035 */
