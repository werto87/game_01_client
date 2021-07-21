#ifndef FEF9CD11_1F15_458C_A025_A3892CE7557A
#define FEF9CD11_1F15_458C_A025_A3892CE7557A

#include <chrono>
#include <game_01_shared_class/serialization.hxx>
#include <optional>
#include <string>
#include <vector>

struct lobby
{
};
struct createGameLobbyWaitForServer
{
};
struct createGameLobby
{
};
struct lobbyWaitForServer
{
};

struct startGame
{
};
struct Lobby
{
  std::string createGameLobbyName;
  std::string createGameLobbyPassword;
  std::string joinGameLobbyName;
  std::string joinGameLobbyPassword;
  bool createCreateGameLobbyClicked = false;
  bool createJoinGameLobbyClicked = false;
  bool logoutButtonClicked = false;
};

struct LobbyWaitForServer
{
};

struct CreateGameLobbyWaitForServer
{
};
struct CreateGameLobbyError
{
  std::string message{};
  bool backToLobbyClicked{};
};
struct CreateGameLobby
{
  shared_class::SetTimerOption timerOption{};
  std::string gameLobbyName{};
  int maxUserInGameLobby{};
  int maxCardValue{};
  std::vector<std::string> accountNamesInGameLobby{};
  bool startGame = false;
  bool sendMaxUserCountClicked = false;
  bool sendMaxCardValueClicked = false;
  bool sendTimerOptionClicked = false;
  bool leaveGameLobby = false;
};

#endif /* FEF9CD11_1F15_458C_A025_A3892CE7557A */
