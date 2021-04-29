#ifndef A76A25AE_A804_47C0_8549_6F15C0EB7035
#define A76A25AE_A804_47C0_8549_6F15C0EB7035

#include "src/ui/uiState.hxx"
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>
#include <boost/optional.hpp>
#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include <variant>
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
  GuiState guiState{};
  GuiState createAccountPopup (CreateAccount &createAccountState);
  GuiState login (Login &loginState);
  GuiState lobby (Lobby &lobbyState);
  void loginErrorPopup ();
  void debug (bool &shouldChangeFontSize);
  void createAccountErrorPopup ();
  void updateFontSize ();
  bool _showDemoWindow = false;
  float _fontScale = 0.5f;
  ImFont *font2{};
  std::string sendMessage{};
  bool _shouldOpenCreateAnAccount{};
};

#endif /* A76A25AE_A804_47C0_8549_6F15C0EB7035 */
