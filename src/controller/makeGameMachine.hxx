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
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<Lobby>                        + event<createGameLobbyWaitForServer>                                                                             = state<CreateGameLobbyWaitForServer>
, state<Lobby>                        + event<shared_class::JoinChannelSuccess>   / reactToJoinChannelSuccess
, state<Lobby>                        + event<shared_class::Message>              / reactToMessage
, state<Lobby>                        + event<draw>                               / (drawLobby,evalLobby,evalChat)         
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/ 
, state<CreateGameLobbyWaitForServer> + on_entry<_>                               / setCreateGameLobbyWaitForServer
, state<CreateGameLobbyWaitForServer> + event<shared_class::CreateGameLobbyError> / showCreateGameLobbyWaitForServerError                                                                      
, state<CreateGameLobbyWaitForServer> + event<shared_class::JoinGameLobbyError>   / showCreateGameLobbyWaitForServerError
, state<CreateGameLobbyWaitForServer> + event<shared_class::JoinGameLobbySuccess>                                                                       = state<CreateGameLobby>
, state<CreateGameLobbyWaitForServer> + event<lobby>                                                                                                    = state<Lobby>
, state<CreateGameLobbyWaitForServer> + event<draw>                               / (drawCreateGameLobbyWaitForServer,evalCreateGameLobbyWaitForServer)         
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
, state<CreateGameLobby>              + event<lobby>                                                                                                    = state<Lobby>
, state<CreateGameLobby>              + event<shared_class::JoinChannelSuccess>   / reactToJoinChannelSuccess
, state<CreateGameLobby>              + event<shared_class::Message>              / reactToMessage
, state<CreateGameLobby>              + event<shared_class::UsersInGameLobby>     / reactToUsersInGameLobby
, state<CreateGameLobby>              + event<draw>                               / (drawCreateGameLobby,evalCreateGameLobby,evalChat)  
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
);
    // clang-format on 
  }
};

#endif /* FC97C1F6_192C_4811_97B2_0968FBE2E707 */
