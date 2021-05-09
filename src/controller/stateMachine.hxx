
#ifndef F74E399C_E6DE_488D_B216_CC0117DE1B1D
#define F74E399C_E6DE_488D_B216_CC0117DE1B1D

#include "src/ui/screen.hxx"
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
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<Lobby>                        + event<createGameLobbyWaitForServer>                                                                       = state<CreateGameLobbyWaitForServer>
, state<Lobby>                        + event<draw>                         /(drawLobby,evalLobby,evalChat)         
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/ 
, state<CreateGameLobbyWaitForServer> + event<createGameLobbyError>                                                                               = state<CreateGameLobbyError>
, state<CreateGameLobbyWaitForServer> + event<gameLobbyData>                                                                                      = state<CreateGameLobby>
, state<CreateGameLobbyWaitForServer> + event<lobby>                                                                                              = state<Lobby>
, state<CreateGameLobbyWaitForServer> + event<draw>                         /(drawCreateGameLobbyWaitForServer,evalCreateGameLobbyWaitForServer)         
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/ 
, state<CreateGameLobbyError>         + event<lobby>                                                                                              = state<Lobby>
, state<CreateGameLobbyError>         + event<draw>                         /(drawCreateGameLobbyError,evalCreateGameLobbyError)  
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
, state<CreateGameLobby>              + event<lobby>                                                                                              = state<Lobby>
, state<CreateGameLobby>              + event<draw>                         /(drawCreateGameLobby,evalCreateGameLobby,evalChat)  
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
);
    // clang-format on 
  }
};

struct LoginStateMachine
{
  auto
  operator() () const noexcept
  {
    using namespace sml;
    return make_transition_table(
        // clang-format off
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<Login>                        + event<loginWaitForServer>                                                                           = state<LoginWaitForServer>
, state<Login>                        + event<createAccount>                                                                                = state<CreateAccount>
, state<Login>                        + event<draw>                        /(drawLogin,evalLogin)         
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<LoginWaitForServer>           + event<loginError>                                                                                   = state<LoginError>
, state<LoginWaitForServer>           + event<loginSuccess>                / (setAccountName,process(makeGameMachine{}))                    = X      
, state<LoginWaitForServer>           + event<login>                                                                                        = state<Login>
, state<LoginWaitForServer>           + event<draw>                        /(drawLoginWaitForServer,evalLoginWaitForServer)         
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<LoginError>                   + event<login>                                                                                        = state<Login>
, state<LoginError>                   + event<draw>                        /(drawLoginError,evalLoginError)     
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<CreateAccount>                + event<createAccountWaitForServer>                                                                   = state<CreateAccountWaitForServer>
, state<CreateAccount>                + event<login>                                                                                        = state<Login>
, state<CreateAccount>                + event<draw>                        /(drawCreateAccount,evalCreateAccount)  
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<CreateAccountWaitForServer>   + event<createAccountError>                                                                           = state<createAccountError>
, state<CreateAccountWaitForServer>   + event<createAccountSuccess>                                                                         = state<CreateAccountSuccess>
, state<CreateAccountWaitForServer>   + event<createAccount>                                                                                = state<CreateAccount>
, state<CreateAccountWaitForServer>   + event<draw>                        /(drawCreateAccountWaitForServer,evalCreateAccountWaitForServer)
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<CreateAccountError>           + event<createAccount>                                                                                = state<CreateAccount>
, state<CreateAccountError>           + event<draw>                        /(drawCreateAccountError,evalCreateAccountError)
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<CreateAccountSuccess>         + event<createAccount>                                                                                = state<CreateAccount>
, state<CreateAccountSuccess>         + event<draw>                        /(drawCreateAccountSuccess,evalCreateAccountSuccess)
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
);
    // clang-format on
  }
};

struct WrapperMachine
{
  auto
  operator() ()
  {
    using namespace sml;
    auto resetGameMachineData = [] (MakeGameMachineData &makeGameMachineData) { makeGameMachineData = MakeGameMachineData{}; };
    return make_transition_table (
        // clang-format off
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<LoginStateMachine>            + event<makeGameMachine>                                             = state<MakeGameMachine>
, state<MakeGameMachine>              + event<logoutSuccess>                                               = state<LoginStateMachine>
, state<MakeGameMachine>              + sml::on_exit<makeGameMachine>            / resetGameMachineData
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
// clang-format on   
    );
  }
};

typedef sml::sm<WrapperMachine, sml::process_queue<std::queue>> StateMachine;


#endif /* F74E399C_E6DE_488D_B216_CC0117DE1B1D */
