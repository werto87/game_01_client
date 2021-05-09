#ifndef B7441788_8126_4373_8FB3_72A5D223C8CB
#define B7441788_8126_4373_8FB3_72A5D223C8CB

#include "imgui.h"
#include "src/controller/state.hxx"

// class Login;
// class CreateAccount;
// class ChatData;
// class CreateGameLobby;
// class Lobby;

void loginScreen (Login &login, float windowSizeX, float windowSizeY, ImFont &biggerFont);
void createAccountScreen (CreateAccount &createAccount, float windowSizeX, float windowSizeY, ImFont &biggerFont);
void chatScreen (ChatData &chatData);
void lobbyScreen (Lobby &lobby, ImFont &, ChatData &chatData);
void createGameLobbyScreen (CreateGameLobby &createGameLobby, ChatData &chatData);
// void waitForServerPopup (WaitForServerPopupState &waitForServerPopupState, float windowSizeX, float windowSizeY, ImFont &biggerFont);
// void messageBoxPopup (MessageBoxPopupState &messageBoxPopupState, float windowSizeX, float windowSizeY, ImFont &biggerFont);

struct draw
{
  float windowSizeX{};
  float windowSizeY{};
  ImFont *biggerFont{};
};
// TODO maybe we can use overloaded lambda and name it draw and overload it on the types to draw
const auto drawLogin = [] (draw const &drawEv, Login &login) { loginScreen (login, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont); };
const auto drawLoginWaitForServer = [] (draw const &, LoginWaitForServer &) {};
const auto drawLoginError = [] (draw const &, LoginError &) {};
const auto drawCreateAccount = [] (draw const &drawEv, CreateAccount &createAccount) { createAccountScreen (createAccount, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont); };
const auto drawCreateAccountWaitForServer = [] (draw const &, CreateAccountWaitForServer &) {};
const auto drawCreateAccountError = [] (draw const &, CreateAccountError &) {};
const auto drawCreateAccountSuccess = [] (draw const &, CreateAccountSuccess &) {};

const auto drawLobby = [] (draw const &drawEv, Lobby &lobby, MakeGameMachineData &makeGameMachineData) { lobbyScreen (lobby, *drawEv.biggerFont, makeGameMachineData.chatData); };
const auto drawCreateGameLobbyWaitForServer = [] (draw const &, CreateGameLobbyWaitForServer &) {};
const auto drawCreateGameLobbyError = [] (draw const &, CreateGameLobbyError &) {};
const auto drawCreateGameLobby = [] (draw const &, CreateGameLobby &createGameLobby, MakeGameMachineData &makeGameMachineData) { createGameLobbyScreen (createGameLobby, makeGameMachineData.chatData); };

#endif /* B7441788_8126_4373_8FB3_72A5D223C8CB */
