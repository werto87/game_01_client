#ifndef D734150A_1DCA_4DAA_BA79_A99F78A5BED4
#define D734150A_1DCA_4DAA_BA79_A99F78A5BED4

#include "src/controller/makeGameMachineState.hxx"
#include "src/controller/sendObject.hxx"
#include "src/controller/stateMachineData.hxx"
#include <boost/sml.hpp>
#include <game_01_shared_class/serialization.hxx>
#include <string>
#include <vector>
namespace sml = boost::sml;

const auto setCreateGameLobbyWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = WaitForServer{};
  using timer = std::chrono::system_clock;
  // TODO we could use it with logout re enable logout after 5 sec
  // waitForServer->buttons = std::vector<Button>{ { .name = "Cancel Create Account", .pressed = false } };
  waitForServer->clock_wait = timer::now ();
};

const auto reactToUsersInGameLobby = [] (shared_class::UsersInGameLobby const &usersInGameLobby, CreateGameLobby &createGameLobby, MakeGameMachineData &makeGameMachineData) {
  createGameLobby.accountNamesInGameLobby.clear ();
  std::ranges::transform (usersInGameLobby.users, std::back_inserter (createGameLobby.accountNamesInGameLobby), [] (shared_class::UserInGameLobby const &user) { return user.accountName; });
  createGameLobby.gameLobbyName = usersInGameLobby.name;
  createGameLobby.maxUserInGameLobby = static_cast<int> (usersInGameLobby.maxUserSize);
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

const auto evalCreateGameLobbyWaitForServer = [] (MessageBoxPopup &messageBoxPopup, MessagesToSendToServer &, sml::back::process<lobby> process_event) {
  if (std::holds_alternative<shared_class::JoinGameLobbyError> (messageBoxPopup.event) || std::holds_alternative<shared_class::CreateGameLobbyError> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed) process_event (lobby{});
    }
};

const auto reactToJoinChannelSuccess = [] (shared_class::JoinChannelSuccess const &joinChannelSuccess, MakeGameMachineData &makeGameMachineData) { makeGameMachineData.chatData.channelMessages.insert_or_assign (joinChannelSuccess.channel, std::vector<std::string>{}); };
const auto reactToMessage = [] (shared_class::Message const &message, MakeGameMachineData &makeGameMachineData) {
  if (makeGameMachineData.chatData.channelMessages.count (message.channel) == 1)
    {
      makeGameMachineData.chatData.channelMessages.at (message.channel).push_back (message.fromAccount + ": " + message.message);
    }
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

#endif /* D734150A_1DCA_4DAA_BA79_A99F78A5BED4 */
