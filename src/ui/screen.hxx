#ifndef B7441788_8126_4373_8FB3_72A5D223C8CB
#define B7441788_8126_4373_8FB3_72A5D223C8CB

#include "src/controller/commonEvent.hxx"
#include "src/controller/loginStateMachineState.hxx"
#include "src/controller/makeGameMachineState.hxx"
#include "src/controller/playTheGameState.hxx"
#include "src/controller/stateMachineData.hxx"
#include <algorithm>
#include <type_traits>
#include <variant>

void chatScreen (ChatData &chatData, bool shouldLockScreen, std::chrono::milliseconds const &time);
void createGameLobbyScreen (CreateGameLobby &createGameLobby, std::optional<WaitForServer> &waitForServer, std::string accountName, ChatData &chatData);
void messageBoxPopupScreen (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont);
void loginScreen (Login &data, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont);
void createAccountScreen (CreateAccount &data, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont);
void lobbyScreen (Lobby &data, std::optional<WaitForServer> &waitForServer, ChatData &chatData);
void gameScreen (Game &data, std::optional<WaitForServer> &waitForServer, std::string const &accountName, ChatData &chatData);

auto const drawLogin = [] (draw const &drawEv, Login &login, MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      loginScreen (login, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
};

auto const drawCreateAccount = [] (draw const &drawEv, CreateAccount &createAccount, std::optional<WaitForServer> &waitForServer, MessageBoxPopup &messageBoxPopup) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      createAccountScreen (createAccount, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
};

auto const drawLobby = [] (draw const &drawEv, Lobby &lobby, std::optional<WaitForServer> &waitForServer, MakeGameMachineData &makeGameMachineData, MessageBoxPopup &messageBoxPopup) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      lobbyScreen (lobby, waitForServer, makeGameMachineData.chatData);
    }
};

auto const drawCreateGameLobby = [] (draw const &drawEv, std::optional<WaitForServer> &waitForServer, MessageBoxPopup &messageBoxPopup, CreateGameLobby &createGameLobby, MakeGameMachineData &makeGameMachineData) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      createGameLobbyScreen (createGameLobby, waitForServer, makeGameMachineData.accountName, makeGameMachineData.chatData);
    }
};

auto const drawGame = [] (draw const &drawEv, Game &game, MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, MakeGameMachineData &makeGameMachineData) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      gameScreen (game, waitForServer, makeGameMachineData.accountName, makeGameMachineData.chatData);
    }
};

#endif /* B7441788_8126_4373_8FB3_72A5D223C8CB */
