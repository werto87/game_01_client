#ifndef F204E65B_7FD7_4265_ACF7_FB534DB042F2
#define F204E65B_7FD7_4265_ACF7_FB534DB042F2

#include <boost/optional.hpp>
#include <imgui.h>
#include <string>
#include <variant>

struct Login
{
  bool loginInProgress = false;
  std::string username;
  std::string password;
};
struct LoginError
{
};
struct CreateAccount
{
  bool createAccountInProgress = false;
  std::string username;
  std::string password;
};
struct CreateAccountSuccess
{
};
struct CreateAccountError
{
};
struct Lobby
{
  boost::optional<std::string> selectedChannelName;
  std::string channelToJoin;
  std::string messageToSendToChannel;
};

using GuiState = std::variant<Login, LoginError, CreateAccount, CreateAccountSuccess, CreateAccountError, Lobby>;

class UiState
{
public:
  void execute (float windowSizeX, float windowSizeY, ImFont &biggerFont);

private:
  GuiState guiState{};
};

#endif /* F204E65B_7FD7_4265_ACF7_FB534DB042F2 */
