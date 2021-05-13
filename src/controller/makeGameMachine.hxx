#ifndef FC97C1F6_192C_4811_97B2_0968FBE2E707
#define FC97C1F6_192C_4811_97B2_0968FBE2E707

#include "src/controller/makeGameMachineAction.hxx"
#include "src/ui/screen.hxx"

struct MakeGameMachine
{
  auto
  operator() () const noexcept
  {
    using namespace sml;
    return make_transition_table (
        // clang-format off
        //TODO add statemachine which wraps this and deals with lobby events and with evalChat 
        //right now we have to write the same code again and again we can send an event and handle it in wrapper
        // TODO add grey out when wait for server in chat
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<Lobby>                        + event<lobbyWaitForServer>                                                                      = state<LobbyWaitForServer>
, state<Lobby>                        + event<shared_class::JoinChannelSuccess>   / reactToJoinChannelSuccess
, state<Lobby>                        + event<shared_class::Message>              / reactToMessage
, state<Lobby>                        + event<draw>                               / (drawLobby,evalLobby,evalChat)         
, state<Lobby>                        + on_entry<_>                               / resetPopupAndWaitForServer
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/ 
, state<LobbyWaitForServer>           + on_entry<_>                               / setLobbyWaitForServer
, state<LobbyWaitForServer>           + event<shared_class::CreateGameLobbyError> / setErrorEvent                                                                      
, state<LobbyWaitForServer>           + event<shared_class::JoinGameLobbyError>   / setErrorEvent
, state<LobbyWaitForServer>           + event<shared_class::JoinGameLobbySuccess>                                                      = state<CreateGameLobby>
, state<LobbyWaitForServer>           + event<lobby>                                                                                   = state<Lobby>
, state<LobbyWaitForServer>           + event<draw>                               / (drawLobby,evalLobbyWaitForServer)         
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
, state<CreateGameLobby>              + on_entry<_>                               / resetPopupAndWaitForServer
, state<CreateGameLobby>              + event<createGameLobbyWaitForServer>                                                            = state<CreateGameLobbyWaitForServer>
, state<CreateGameLobby>              + event<lobby>                                                                                   = state<Lobby>
, state<CreateGameLobby>              + event<shared_class::JoinChannelSuccess>   / reactToJoinChannelSuccess
, state<CreateGameLobby>              + event<shared_class::Message>              / reactToMessage
, state<CreateGameLobby>              + event<shared_class::UsersInGameLobby>     / reactToUsersInGameLobby
, state<CreateGameLobby>              + event<draw>                               / (drawCreateGameLobby,evalCreateGameLobby,evalChat)  
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
, state<CreateGameLobbyWaitForServer>           + on_entry<_>                                               / setCreateGameLobbyWaitForServer
, state<CreateGameLobbyWaitForServer>           + event<shared_class::SetMaxUserSizeInCreateGameLobbyError> / setErrorEvent                                                                      
, state<CreateGameLobbyWaitForServer>           + event<shared_class::MaxUserSizeInCreateGameLobby>                                                                 = state<CreateGameLobby>
, state<CreateGameLobbyWaitForServer>           + event<lobby>                                                                                                      = state<Lobby>
, state<CreateGameLobbyWaitForServer>           + event<draw>                                               / (drawCreateGameLobby,evalCreateGameLobbyWaitForServer)         
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
);
    // clang-format on 
  }
};

#endif /* FC97C1F6_192C_4811_97B2_0968FBE2E707 */
