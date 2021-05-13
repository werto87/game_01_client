#ifndef B7441788_8126_4373_8FB3_72A5D223C8CB
#define B7441788_8126_4373_8FB3_72A5D223C8CB

#include "src/controller/commonEvent.hxx"
#include "src/controller/loginStateMachineState.hxx"
#include "src/controller/makeGameMachineState.hxx"
#include "src/controller/stateMachineData.hxx"
#include <algorithm>
#include <type_traits>
#include <variant>

void chatScreen (ChatData &chatData);
void createGameLobbyScreen (CreateGameLobby &createGameLobby, std::string accountName, ChatData &chatData);
void messageBoxPopupScreen (MessageBoxPopup &messageBoxPopup, float windowWidth, float windowHeight, ImFont &biggerFont);
void loginScreen (Login &data, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont);
void createAccountScreen (CreateAccount &data, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont);
void lobbyScreen (Lobby &data, std::optional<WaitForServer> &waitForServer, ChatData &chatData);

const auto drawLogin = [] (draw const &drawEv, Login &login, MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      loginScreen (login, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
};

const auto drawCreateAccount = [] (draw const &drawEv, CreateAccount &createAccount, std::optional<WaitForServer> &waitForServer, MessageBoxPopup &messageBoxPopup) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      createAccountScreen (createAccount, waitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
};

const auto drawLobby = [] (draw const &drawEv, Lobby &lobby, std::optional<WaitForServer> &waitForServer, MakeGameMachineData &makeGameMachineData, MessageBoxPopup &messageBoxPopup) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      lobbyScreen (lobby, waitForServer, makeGameMachineData.chatData);
    }
};

const auto drawCreateGameLobby = [] (draw const &, CreateGameLobby &createGameLobby, MakeGameMachineData &makeGameMachineData) { createGameLobbyScreen (createGameLobby, makeGameMachineData.accountName, makeGameMachineData.chatData); };

const auto showMessageBoxPopup = [] (draw const &drawEv, MessageBoxPopup &messageBoxPopup) { messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont); };

#endif /* B7441788_8126_4373_8FB3_72A5D223C8CB */
