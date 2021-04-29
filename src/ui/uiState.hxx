#ifndef F204E65B_7FD7_4265_ACF7_FB534DB042F2
#define F204E65B_7FD7_4265_ACF7_FB534DB042F2

#include <boost/optional.hpp>
#include <string>
#include <variant>

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
using GuiState = std::variant<Login, CreateAccount, Lobby>;

class TestStateMachine
{
public:
  void execute (float windowSizeX, float windowSizeY);

private:
  GuiState login (Login &loginState, float windowSizeX, float windowSizeY);
  GuiState createAccountPopup (CreateAccount &createAccountState, float windowSizeX, float windowSizeY);
  GuiState lobby (Lobby &lobbyState, float windowSizeX, float windowSizeY);
  GuiState guiState{};
};

#endif /* F204E65B_7FD7_4265_ACF7_FB534DB042F2 */
