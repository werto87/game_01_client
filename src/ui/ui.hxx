#ifndef A76A25AE_A804_47C0_8549_6F15C0EB7035
#define A76A25AE_A804_47C0_8549_6F15C0EB7035
#include "src/controller/stateMachine.hxx"
#include "src/webservice/webservice.hxx"
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include <variant>

#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

enum struct SoftKeyboardState
{
  smallChars,
  bigChars,
  specialChars1,
  specialChars2
};

class ImGuiExample : public Magnum::Platform::Application
{

public:
  ImGuiExample (const Arguments &arguments, bool _isTouch, std::string const &websocketAddress);
  void drawEvent () override;
  void viewportEvent (ViewportEvent &event) override;
  void keyPressEvent (KeyEvent &event) override;
  void keyReleaseEvent (KeyEvent &event) override;
  void mousePressEvent (MouseEvent &event) override;
  void mouseReleaseEvent (MouseEvent &event) override;
  void mouseMoveEvent (MouseMoveEvent &event) override;
  void mouseScrollEvent (MouseScrollEvent &event) override;
  void textInputEvent (TextInputEvent &event) override;
  void updateFontSize ();

  Magnum::ImGuiIntegration::Context _imgui{ Magnum::NoCreate };

private:
  ImFont *smallFont{};
  ImFont *bigFont{};
  my_logger logger{};
  StateMachine _stateMachine;
  boost::asio::io_context ioContext{};
  Webservice webservice;
  MessagesToSendToServer _messagesToSendToServer{};
  bool isTouch = false;
  boost::optional<std::string &> textInputString{};

  SoftKeyboardState softKeyboardState{};
  // debug--------------------------------------------
  void debug ();
  void createAccountErrorPopup ();

  bool _showDemoWindow = false;
  float _fontScale = 0.5f;
  std::string sendMessage{};
  // debug--------------------------------------------
};

#endif /* A76A25AE_A804_47C0_8549_6F15C0EB7035 */
