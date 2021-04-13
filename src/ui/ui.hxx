#ifndef A76A25AE_A804_47C0_8549_6F15C0EB7035
#define A76A25AE_A804_47C0_8549_6F15C0EB7035

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>
#include <deque>
#include <memory>
#include <string>
class ImGuiExample : public Magnum::Platform::Application
{

public:
  ImGuiExample (const Arguments &arguments);

  void drawEvent () override;

  void viewportEvent (ViewportEvent &event) override;

  void keyPressEvent (KeyEvent &event) override;
  void keyReleaseEvent (KeyEvent &event) override;

  void mousePressEvent (MouseEvent &event) override;
  void mouseReleaseEvent (MouseEvent &event) override;
  void mouseMoveEvent (MouseMoveEvent &event) override;
  void mouseScrollEvent (MouseScrollEvent &event) override;
  void textInputEvent (TextInputEvent &event) override;

  Magnum::ImGuiIntegration::Context _imgui{ Magnum::NoCreate };

private:
  void createAccountPopup (bool &shouldOpenCreateAnAccount);
  void login ();
  void loginErrorPopup ();
  void debug (bool &shouldChangeFontSize);
  void updateFontSize ();
  bool _showDemoWindow = false;
  bool _showCreateAccount = false;
  float _fontScale = 0.5f;
  ImFont *font2{};
  std::string password{};
  std::string username{};
  std::string create_password{};
  std::string create_username{};
};

#endif /* A76A25AE_A804_47C0_8549_6F15C0EB7035 */
