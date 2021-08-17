#ifndef D734150A_1DCA_4DAA_BA79_A99F78A5BED4
#define D734150A_1DCA_4DAA_BA79_A99F78A5BED4

#include "src/controller/makeGameMachineState.hxx"
#include "src/controller/sendObject.hxx"
#include "src/controller/stateMachineData.hxx"
#include <boost/sml.hpp>
#include <game_01_shared_class/serialization.hxx>
#include <string>
#include <sys/types.h>
#include <vector>
namespace sml = boost::sml;

auto const setLobbyWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, boost::optional<std::string &> &textInputString) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = WaitForServer{};
  using timer = std::chrono::system_clock;
  waitForServer->clock_wait = timer::now ();
  textInputString = {};
};

auto const resetCreateGameLobby = [] (CreateGameLobby &createGameLobby, boost::optional<std::string &> &textInputString) {
  createGameLobby = {};
  textInputString = {};
};

auto const setCreateGameLobbyWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, boost::optional<std::string &> &textInputString) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = WaitForServer{};
  using timer = std::chrono::system_clock;
  waitForServer->clock_wait = timer::now ();
  textInputString = {};
};

auto const setTimerOption = [] (shared_class::SetTimerOption const &setTimerOptionEv, CreateGameLobby &createGameLobby) { createGameLobby.timerOption = setTimerOptionEv; };

auto const reactToUsersInGameLobby = [] (shared_class::UsersInGameLobby const &usersInGameLobby, CreateGameLobby &createGameLobby) {
  createGameLobby.accountNamesInGameLobby.clear ();
  std::transform (usersInGameLobby.users.begin (), usersInGameLobby.users.end (), std::back_inserter (createGameLobby.accountNamesInGameLobby), [] (shared_class::UserInGameLobby const &user) { return user.accountName; });
  createGameLobby.gameLobbyName = usersInGameLobby.name;
  createGameLobby.maxUserInGameLobby = static_cast<int> (usersInGameLobby.maxUserSize);
  createGameLobby.maxCardValue = static_cast<u_int16_t> (usersInGameLobby.durakGameOption.maxCardValue);
};

auto const reactToMaxUserSizeInCreateGameLobby = [] (shared_class::MaxUserSizeInCreateGameLobby const &maxUserSizeInCreateGameLobby, CreateGameLobby &createGameLobby) { createGameLobby.maxUserInGameLobby = static_cast<int> (maxUserSizeInCreateGameLobby.maxUserSize); };
auto const reactToMaxCardValueInCreateGameLobby = [] (shared_class::MaxCardValueInCreateGameLobby const &maxCardValueInCreateGameLobby, CreateGameLobby &createGameLobby) { createGameLobby.maxCardValue = static_cast<u_int16_t> (maxCardValueInCreateGameLobby.maxCardValue); };

auto const evalLobby = [] (Lobby &lobby, MessagesToSendToServer &messagesToSendToServer, MakeGameMachineData &makeGameMachineData, sml::back::process<lobbyWaitForServer> process_event) {
  if (lobby.logoutButtonClicked)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LogoutAccount{});
      process_event (lobbyWaitForServer{});
    }
  else if (lobby.createCreateGameLobbyClicked && not lobby.createGameLobbyName.empty ())
    {
      auto createGameLobby = shared_class::CreateGameLobby{};
      createGameLobby.name = lobby.createGameLobbyName;
      createGameLobby.password = lobby.createGameLobbyPassword;
      sendObject (messagesToSendToServer.messagesToSendToServer, createGameLobby);
      process_event (lobbyWaitForServer{});
    }
  else if (lobby.createJoinGameLobbyClicked && not lobby.joinGameLobbyName.empty ())
    {
      auto joinGameLobby = shared_class::JoinGameLobby{};
      joinGameLobby.name = lobby.joinGameLobbyName;
      joinGameLobby.password = lobby.joinGameLobbyPassword;
      sendObject (messagesToSendToServer.messagesToSendToServer, joinGameLobby);
      process_event (lobbyWaitForServer{});
    }
  else if (makeGameMachineData.chatData.joinChannelClicked && not makeGameMachineData.chatData.channelToJoin.empty ())
    {
      auto joinChannel = shared_class::JoinChannel{};
      joinChannel.channel = makeGameMachineData.chatData.channelToJoin;
      sendObject (messagesToSendToServer.messagesToSendToServer, joinChannel);
      makeGameMachineData.chatData.channelToJoin.clear ();
      process_event (lobbyWaitForServer{});
    }
  else if (makeGameMachineData.chatData.sendMessageClicked && makeGameMachineData.chatData.selectedChannelName && not makeGameMachineData.chatData.selectedChannelName->empty () && not makeGameMachineData.chatData.messageToSendToChannel.empty ())
    {
      auto broadCastMessage = shared_class::BroadCastMessage{};
      broadCastMessage.channel = makeGameMachineData.chatData.selectedChannelName.value ();
      broadCastMessage.message = makeGameMachineData.chatData.messageToSendToChannel;
      sendObject (messagesToSendToServer.messagesToSendToServer, broadCastMessage);
      makeGameMachineData.chatData.messageToSendToChannel.clear ();
      process_event (lobbyWaitForServer{});
    }
};

auto const evalLobbyWaitForServer = [] (MessageBoxPopup &messageBoxPopup, MessagesToSendToServer &, sml::back::process<lobby> process_event) {
  if (std::holds_alternative<shared_class::JoinGameLobbyError> (messageBoxPopup.event) || std::holds_alternative<shared_class::CreateGameLobbyError> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed) process_event (lobby{});
    }
};

auto const evalCreateGameLobbyWaitForServer = [] (MessageBoxPopup &messageBoxPopup, MessagesToSendToServer &, sml::back::process<lobby> process_event) {
  if (std::holds_alternative<shared_class::SetMaxUserSizeInCreateGameLobbyError> (messageBoxPopup.event) || std::holds_alternative<shared_class::SetMaxCardValueInCreateGameLobbyError> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed) process_event (lobby{});
    }
};

auto const reactToJoinChannelSuccess = [] (shared_class::JoinChannelSuccess const &joinChannelSuccess, MakeGameMachineData &makeGameMachineData) { makeGameMachineData.chatData.channelMessages.insert_or_assign (joinChannelSuccess.channel, std::vector<std::string>{}); };
auto const reactToMessage = [] (shared_class::Message const &message, MakeGameMachineData &makeGameMachineData) {
  if (makeGameMachineData.chatData.channelMessages.count (message.channel) == 1)
    {
      makeGameMachineData.chatData.channelMessages.at (message.channel).push_back (message.fromAccount + ": " + message.message);
    }
};

auto const evalCreateGameLobby = [] (CreateGameLobby &createGameLobby, MakeGameMachineData &makeGameMachineData, MessagesToSendToServer &messagesToSendToServer, sml::back::process<lobby, createGameLobbyWaitForServer> process_event) {
  if (createGameLobby.leaveGameLobby)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LeaveGameLobby{});
      process_event (lobby{});
    }
  else if (createGameLobby.sendMaxUserCountClicked)
    {
      process_event (createGameLobbyWaitForServer{});
      auto setMaxUserSizeInCreateGameLobby = shared_class::SetMaxUserSizeInCreateGameLobby{};
      setMaxUserSizeInCreateGameLobby.createGameLobbyName = createGameLobby.gameLobbyName;
      setMaxUserSizeInCreateGameLobby.maxUserSize = static_cast<size_t> (createGameLobby.maxUserInGameLobby);
      sendObject (messagesToSendToServer.messagesToSendToServer, setMaxUserSizeInCreateGameLobby);
    }
  else if (createGameLobby.sendMaxCardValueClicked)
    {
      process_event (createGameLobbyWaitForServer{});
      auto setMaxCardValueInCreateGameLobby = shared_class::SetMaxCardValueInCreateGameLobby{};
      setMaxCardValueInCreateGameLobby.createGameLobbyName = createGameLobby.gameLobbyName;
      setMaxCardValueInCreateGameLobby.maxCardValue = static_cast<u_int16_t> (createGameLobby.maxCardValue);
      sendObject (messagesToSendToServer.messagesToSendToServer, setMaxCardValueInCreateGameLobby);
    }
  else if (createGameLobby.sendTimerOptionClicked)
    {
      process_event (createGameLobbyWaitForServer{});
      sendObject (messagesToSendToServer.messagesToSendToServer, createGameLobby.timerOption);
    }
  else if (makeGameMachineData.chatData.joinChannelClicked && not makeGameMachineData.chatData.channelToJoin.empty ())
    {
      auto joinChannel = shared_class::JoinChannel{};
      joinChannel.channel = makeGameMachineData.chatData.channelToJoin;
      sendObject (messagesToSendToServer.messagesToSendToServer, joinChannel);
      makeGameMachineData.chatData.channelToJoin.clear ();
      process_event (createGameLobbyWaitForServer{});
    }
  else if (makeGameMachineData.chatData.sendMessageClicked && makeGameMachineData.chatData.selectedChannelName && not makeGameMachineData.chatData.selectedChannelName->empty () && not makeGameMachineData.chatData.messageToSendToChannel.empty ())
    {
      auto broadCastMessage = shared_class::BroadCastMessage{};
      broadCastMessage.channel = makeGameMachineData.chatData.selectedChannelName.value ();
      broadCastMessage.message = makeGameMachineData.chatData.messageToSendToChannel;
      sendObject (messagesToSendToServer.messagesToSendToServer, broadCastMessage);
      makeGameMachineData.chatData.messageToSendToChannel.clear ();
      process_event (createGameLobbyWaitForServer{});
    }
  else if (createGameLobby.startGame)
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::CreateGame{});
      process_event (createGameLobbyWaitForServer{});
    }
};

#endif /* D734150A_1DCA_4DAA_BA79_A99F78A5BED4 */
