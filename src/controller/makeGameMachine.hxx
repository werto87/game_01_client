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
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<Lobby>                        + event<lobbyWaitForServer>                                                                                         = state<LobbyWaitForServer>
, state<Lobby>                        + event<draw>                                               / (drawLobby,evalLobby)         
, state<Lobby>                        + on_entry<_>                                               / resetPopupAndWaitForServer
, state<Lobby>                        + event<shared_class::Message>                              / reactToMessage        
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/ 
, state<LobbyWaitForServer>           + on_entry<_>                                               / setLobbyWaitForServer
, state<LobbyWaitForServer>           + event<shared_class::Message>                              / reactToMessage                                        = state<Lobby>
, state<LobbyWaitForServer>           + event<shared_class::JoinChannelSuccess>                   / reactToJoinChannelSuccess                             = state<Lobby>
, state<LobbyWaitForServer>           + event<shared_class::CreateGameLobbyError>                 / setErrorEvent                                                                      
, state<LobbyWaitForServer>           + event<shared_class::JoinGameLobbyError>                   / setErrorEvent
, state<LobbyWaitForServer>           + event<shared_class::JoinGameLobbySuccess>                                                                         = state<CreateGameLobby>
, state<LobbyWaitForServer>           + event<lobby>                                                                                                      = state<Lobby>
, state<LobbyWaitForServer>           + event<draw>                                               / (drawLobby,evalLobbyWaitForServer)         
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
, state<CreateGameLobby>              + on_entry<_>                                               / resetPopupAndWaitForServer
, state<CreateGameLobby>              + event<createGameLobbyWaitForServer>                                                                               = state<CreateGameLobbyWaitForServer>
, state<CreateGameLobby>              + event<lobby>                                                                                                      = state<Lobby>
, state<CreateGameLobby>              + event<shared_class::Message>                              / reactToMessage
, state<CreateGameLobby>              + event<shared_class::UsersInGameLobby>                     / reactToUsersInGameLobby
, state<CreateGameLobby>              + event<draw>                                               / (drawCreateGameLobby,evalCreateGameLobby)  
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
, state<CreateGameLobbyWaitForServer> + on_entry<_>                                               / setCreateGameLobbyWaitForServer
, state<CreateGameLobbyWaitForServer> + event<shared_class::Message>                              / reactToMessage                                         = state<CreateGameLobby>
, state<CreateGameLobbyWaitForServer> + event<shared_class::JoinChannelSuccess>                   / reactToJoinChannelSuccess                              = state<CreateGameLobby>
, state<CreateGameLobbyWaitForServer> + event<shared_class::SetMaxUserSizeInCreateGameLobbyError> / setErrorEvent                                                                      
, state<CreateGameLobbyWaitForServer> + event<shared_class::MaxUserSizeInCreateGameLobby>                                                                  = state<CreateGameLobby>
, state<CreateGameLobbyWaitForServer> + event<lobby>                                                                                                       = state<Lobby>
, state<CreateGameLobbyWaitForServer> + event<draw>                                               / (drawCreateGameLobby,evalCreateGameLobbyWaitForServer)         
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

);
    // clang-format on 
  }
};

#endif /* FC97C1F6_192C_4811_97B2_0968FBE2E707 */
