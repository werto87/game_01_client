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

void chatScreen (ChatData &chatData, bool shouldLockScreen, std::chrono::milliseconds const &time, draw const &drawEv, boost::optional<std::string &> &selectedString);
void createGameLobbyScreen (CreateGameLobby &createGameLobby, std::optional<WaitForServer> &waitForServer, std::string accountName, ChatData &chatData, draw const &drawEv, boost::optional<std::string &> &selectedString);
void messageBoxPopupScreen (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, draw const &drawEv);
void loginScreen (Login &data, std::optional<WaitForServer> &waitForServer, draw const &drawEv, boost::optional<std::string &> &selectedString);
void createAccountScreen (CreateAccount &data, std::optional<WaitForServer> &waitForServer, draw const &drawEv, boost::optional<std::string &> &selectedString);
void lobbyScreen (Lobby &data, std::optional<WaitForServer> &waitForServer, ChatData &chatData, draw const &drawEv, boost::optional<std::string &> &selectedString);
void gameScreen (Game &data, std::optional<WaitForServer> &waitForServer, std::string const &accountName, ChatData &chatData, draw const &drawEv, boost::optional<std::string &> &selectedString);

auto const drawLogin = [] (draw const &drawEv, Login &login, MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, boost::optional<std::string &> &selectedString) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv);
    }
  else
    {
      loginScreen (login, waitForServer, drawEv, selectedString);
    }
};

auto const drawCreateAccount = [] (draw const &drawEv, CreateAccount &createAccount, std::optional<WaitForServer> &waitForServer, MessageBoxPopup &messageBoxPopup, boost::optional<std::string &> &selectedString) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv);
    }
  else
    {
      createAccountScreen (createAccount, waitForServer, drawEv, selectedString);
    }
};

auto const drawLobby = [] (draw const &drawEv, Lobby &lobby, std::optional<WaitForServer> &waitForServer, MakeGameMachineData &makeGameMachineData, MessageBoxPopup &messageBoxPopup, boost::optional<std::string &> &selectedString) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv);
    }
  else
    {
      lobbyScreen (lobby, waitForServer, makeGameMachineData.chatData, drawEv, selectedString);
    }
};

auto const drawCreateGameLobby = [] (draw const &drawEv, std::optional<WaitForServer> &waitForServer, MessageBoxPopup &messageBoxPopup, CreateGameLobby &createGameLobby, MakeGameMachineData &makeGameMachineData, boost::optional<std::string &> &selectedString) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv);
    }
  else
    {
      createGameLobbyScreen (createGameLobby, waitForServer, makeGameMachineData.accountName, makeGameMachineData.chatData, drawEv, selectedString);
    }
};

auto const drawGame = [] (draw const &drawEv, Game &game, MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, MakeGameMachineData &makeGameMachineData, boost::optional<std::string &> &selectedString) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, waitForServer, drawEv);
    }
  else
    {
      gameScreen (game, waitForServer, makeGameMachineData.accountName, makeGameMachineData.chatData, drawEv, selectedString);
    }
};

#endif /* B7441788_8126_4373_8FB3_72A5D223C8CB */
