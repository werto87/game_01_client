#ifndef A76A25AE_A804_47C0_8549_6F15C0EB7035
#define A76A25AE_A804_47C0_8549_6F15C0EB7035

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

  // TODO play around with variant as state machine
  struct Login
  {
    bool shouldOpenCreateAnAccount;
    std::string username;
    std::string password;
  };
  struct CreateAccount
  {
    std::string username;
    std::string password;
  };
  struct Lobby
  {
    boost::optional<std::string> selectedChannelName;
    std::string channelToJoin;
    std::string messageToSendToChannel;
  };

  typedef std::variant<Login, CreateAccount, Lobby> GuiState;

  GuiState guiState{};

private:
  GuiState createAccountPopup (CreateAccount &createAccountState);
  GuiState login (Login &loginState);
  void loginErrorPopup ();
  GuiState lobby (Lobby &lobbyState);
  void debug (bool &shouldChangeFontSize);
  void createAccountErrorPopup ();
  void updateFontSize ();
  bool _showDemoWindow = false;
  bool _showCreateAccount = false;
  float _fontScale = 0.5f;
  ImFont *font2{};
  Login loginData{};
  Lobby lobbyData{};
  CreateAccount createAccountData{};
  std::string sendMessage{};
  bool _shouldOpenCreateAnAccount{};
};

#endif /* A76A25AE_A804_47C0_8549_6F15C0EB7035 */