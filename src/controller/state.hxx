#ifndef EBE26351_4857_4382_9622_DE0400244EE9
#define EBE26351_4857_4382_9622_DE0400244EE9

#include <boost/sml.hpp>
#include <cassert>
#include <confu_boost/confuBoost.hxx>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <functional>
#include <game_01_shared_class/serialization.hxx>
#include <iostream>
#include <map>
#include <pipes/pipes.hpp>
#include <queue>
#include <variant>

namespace sml = boost::sml;

template <typename TypeToSend>
void
sendObject (std::deque<std::string> &msgToSend, TypeToSend const &typeToSend)
{
  msgToSend.push_back (confu_soci::typeNameWithOutNamespace (typeToSend) + '|' + confu_boost::toString (typeToSend));
}

struct loginStateMachine
{
};
struct makeGameMachine
{
  std::string accountName;
};

struct logoutSuccess
{
};

struct lobby
{
};
struct createGameLobbyWaitForServer
{
};
struct createGameLobby
{
};
struct createGameLobbyError
{
  std::string message{};
};
struct gameLobbyData
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
struct CreateGameLobbyWaitForServer
{
  std::string message{};
  bool backToLobbyClicked{};
};
struct CreateGameLobbyError
{
  std::string message{};
  bool backToLobbyClicked{};
};
struct CreateGameLobby
{
  std::string accountName{};
  std::string gameLobbyName{};
  int maxUserInGameLobby{};
  int maxUserInGameLobbyToSend{};
  std::vector<std::string> accountNamesInGameLobby{};
  bool sendMaxUserCountClicked = false;
  bool leaveGameLobby = false;
};

struct ChatData
{
  std::string
  selectChannelComboBoxName ()
  {
    return selectedChannelName.value_or ("Select Channel");
  }
  std::vector<std::string> const &
  messagesForChannel (std::string const &channel)
  {
    return channelMessages.at (channel);
  }

  std::vector<std::string>
  channelNames ()
  {
    auto result = std::vector<std::string>{};
    channelMessages >>= pipes::transform ([] (auto const &channelAndMessages) { return std::get<0> (channelAndMessages); }) >>= pipes::push_back (result);
    return result;
  }

  std::optional<std::string> selectedChannelName;
  std::string channelToJoin;
  std::string messageToSendToChannel;
  std::map<std::string, std::vector<std::string>> channelMessages{};
  bool joinChannelClicked = false;
  bool sendMessageClicked = false;
};

struct MakeGameMachineData
{
  ChatData chatData{};
  std::string accountName{};
  std::deque<std::string> messagesToSendToServer{};
};

struct MessagesToSendToServer
{
  std::deque<std::string> messagesToSendToServer{};
};

struct login
{
};
struct loginWaitForServer
{
};
struct loginError
{
  std::string message{};
};
struct loginSuccess
{
  std::string accountName{};
};
struct createAccount
{
};
struct createAccountWaitForServer
{
};
struct createAccountError
{
  std::string message{};
};
struct createAccountSuccess
{
};

struct Login
{
  std::string accountName{};
  std::string password{};
  bool createAccountClicked = false;
  bool loginClicked = false;
};
struct LoginWaitForServer
{
  std::string message{};
  bool backToLoginClicked{};
};
struct LoginError
{
  std::string message{};
  bool backToLoginClicked{};
};
struct CreateAccount
{
  std::string accountName{};
  std::string password{};
  bool createAccountClicked = false;
  bool backToLoginClicked{};
};
struct CreateAccountWaitForServer
{
  std::string message{};
  bool backToAccountClicked{};
};
struct CreateAccountError
{
  std::string message{};
  bool backToAccountClicked{};
};
struct CreateAccountSuccess
{
  std::string message{};
  bool backToAccountClicked{};
};

const auto evalLogin = [] (Login &login, MessagesToSendToServer &messagesToSendToServer, sml::back::process<loginWaitForServer, createAccount> process_event) {
  if (login.loginClicked)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LoginAccount{ .accountName = login.accountName, .password = login.password });
      process_event (loginWaitForServer{});
    }
  if (login.createAccountClicked) process_event (createAccount{});
};

const auto evalLoginWaitForServer = [] (LoginWaitForServer &loginWaitForServer, sml::back::process<login> process_event) {
  if (loginWaitForServer.backToLoginClicked) process_event (login{});
};

const auto evalLoginError = [] (LoginError &loginError, sml::back::process<login> process_event) {
  if (loginError.backToLoginClicked) process_event (login{});
};

const auto evalCreateAccount = [] (CreateAccount &createAccount, MessagesToSendToServer &messagesToSendToServer, sml::back::process<createAccountWaitForServer, login> process_event) {
  if (createAccount.createAccountClicked)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::CreateAccount{ .accountName = createAccount.accountName, .password = createAccount.password });
      process_event (createAccountWaitForServer{});
    }
  if (createAccount.backToLoginClicked) process_event (login{});
};

const auto evalCreateAccountWaitForServer = [] (CreateAccountWaitForServer &createAccountWaitForServer, sml::back::process<createAccount> process_event) {
  if (createAccountWaitForServer.backToAccountClicked) process_event (createAccount{});
};

const auto evalCreateAccountError = [] (CreateAccountError &createAccountError, sml::back::process<createAccount> process_event) {
  if (createAccountError.backToAccountClicked) process_event (createAccount{});
};
const auto evalCreateAccountSuccess = [] (CreateAccountSuccess &createAccountSuccess, sml::back::process<createAccount> process_event) {
  if (createAccountSuccess.backToAccountClicked) process_event (createAccount{});
};

const auto evalLobby = [] (Lobby &lobby, MessagesToSendToServer &messagesToSendToServer, sml::back::process<createGameLobbyWaitForServer> process_event) {
  if (lobby.logoutButtonClicked)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LogoutAccount{});
    }
  if (lobby.createCreateGameLobbyClicked && not lobby.createGameLobbyName.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::CreateGameLobby{ .name = lobby.createGameLobbyName, .password = lobby.createGameLobbyPassword });
      process_event (createGameLobbyWaitForServer{});
    }
  if (lobby.createJoinGameLobbyClicked && not lobby.joinGameLobbyName.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::JoinGameLobby{ .name = lobby.joinGameLobbyName, .password = lobby.joinGameLobbyPassword });
      process_event (createGameLobbyWaitForServer{});
    }
};

const auto evalCreateGameLobbyWaitForServer = [] (CreateGameLobbyWaitForServer &createGameLobbyWaitForServer, MessagesToSendToServer &messagesToSendToServer, sml::back::process<lobby> process_event) {
  if (createGameLobbyWaitForServer.backToLobbyClicked) process_event (lobby{});
};

const auto evalCreateGameLobbyError = [] (CreateGameLobbyError &createGameLobbyError, MessagesToSendToServer &messagesToSendToServer, sml::back::process<lobby> process_event) {
  if (createGameLobbyError.backToLobbyClicked) process_event (lobby{});
};

const auto evalCreateGameLobby = [] (CreateGameLobby &createGameLobby, MessagesToSendToServer &messagesToSendToServer, sml::back::process<lobby> process_event) {
  if (createGameLobby.leaveGameLobby)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LeaveGameLobby{});
      process_event (lobby{});
    }
  if (createGameLobby.sendMaxUserCountClicked)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::SetMaxUserSizeInCreateGameLobby{ .createGameLobbyName = createGameLobby.gameLobbyName, .maxUserSize = static_cast<size_t> (createGameLobby.maxUserInGameLobby) });
      // TODO button should be greyed out while request runs
    }
};

const auto evalChat = [] (MakeGameMachineData &makeGameMachineData, MessagesToSendToServer &messagesToSendToServer) {
  if (makeGameMachineData.chatData.joinChannelClicked && not makeGameMachineData.chatData.channelToJoin.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::JoinChannel{ .channel = makeGameMachineData.chatData.channelToJoin });
      makeGameMachineData.chatData.channelToJoin.clear ();
    }
  if (makeGameMachineData.chatData.sendMessageClicked && makeGameMachineData.chatData.selectedChannelName && not makeGameMachineData.chatData.selectedChannelName->empty () && not makeGameMachineData.chatData.messageToSendToChannel.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::BroadCastMessage{ .channel = makeGameMachineData.chatData.selectedChannelName.value (), .message = makeGameMachineData.chatData.messageToSendToChannel });
      makeGameMachineData.chatData.messageToSendToChannel.clear ();
    }
};

const auto setAccountName = [] (loginSuccess const &loginSuccessEv, MakeGameMachineData &makeGameMachineData) { makeGameMachineData.accountName = loginSuccessEv.accountName; };

#endif /* EBE26351_4857_4382_9622_DE0400244EE9 */
