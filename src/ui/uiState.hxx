#ifndef AF442575_6D32_495D_85A1_C9B7994A72E9
#define AF442575_6D32_495D_85A1_C9B7994A72E9

#include <boost/optional.hpp>
#include <cstddef>
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

  std::string gameLobbyToCreateName;
  std::string gameLobbyToCreatePassword;
  std::string gameLobbyToJoinName;
  std::string gameLobbyToJoinPassword;
};
struct LobbyForCreatingAGame
{
  int setMaxAccountInGameLobby{};
};

struct WantToRelogPopup
{
};

struct RelogToError
{
};

using GuiState = std::variant<Login, LoginError, CreateAccount, CreateAccountSuccess, CreateAccountError, Lobby, LobbyForCreatingAGame, WantToRelogPopup, RelogToError>;

class UiState
{
public:
  void execute (float windowSizeX, float windowSizeY, ImFont &biggerFont);

private:
  GuiState guiState{};
  // ChatState chatState{};
};

void disabledSmallButton (std::string const &buttonName);
void disabledButton (std::string const &buttonName);

#endif /* AF442575_6D32_495D_85A1_C9B7994A72E9 */

GuiState lobby (Lobby &lobbyState, ImFont &);
