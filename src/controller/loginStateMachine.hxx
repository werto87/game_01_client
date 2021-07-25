#ifndef C9747240_569B_4FC4_A287_AFBC41ECAF3C
#define C9747240_569B_4FC4_A287_AFBC41ECAF3C

#include "src/controller/loginStateMachineAction.hxx"
#include "src/ui/screen.hxx"
#include <game_01_shared_class/serialization.hxx>
struct LoginStateMachine
{
  auto
  operator() () const noexcept
  {
    using namespace sml;
    return make_transition_table (
        // clang-format off
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<Login>                        + on_entry<_>                               / resetPopupAndWaitForServer
, state<Login>                        + event<loginWaitForServer>                                                                       = state<LoginWaitForServer>
, state<Login>                        + event<createAccount>                                                                            = state<CreateAccount>
, state<Login>                        + event<draw>                               / (drawLogin,evalLogin)         
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<LoginWaitForServer>           + on_entry<_>                               / setLoginWaitForServer
, state<LoginWaitForServer>           + event<shared_class::LoginAccountError>    / setErrorEvent
, state<LoginWaitForServer>           + event<shared_class::WantToRelog>          / (setAccountName,showWantToRelog)
, state<LoginWaitForServer>           + event<shared_class::LoginAccountSuccess>  / (setAccountName,process(makeGameMachine{}))         = X      
, state<LoginWaitForServer>           + event<shared_class::RelogToCreateGameLobbySuccess>       / process(goToCreateGameLobby{})       = X
, state<LoginWaitForServer>           + event<shared_class::RelogToGameSuccess>    / process(goToGame{})                              = X
, state<LoginWaitForServer>           + event<shared_class::RelogToError>          / setRelogToError
, state<LoginWaitForServer>           + event<login>                                                                                    = state<Login>
, state<LoginWaitForServer>           + event<draw>                               / (drawLogin,evalLoginWaitForServer)         
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<CreateAccount>                + on_entry<_>                               / resetPopupAndWaitForServer
, state<CreateAccount>                + event<createAccountWaitForServer>                                                               = state<CreateAccountWaitForServer>
, state<CreateAccount>                + event<login>                                                                                    = state<Login>
, state<CreateAccount>                + event<draw>                               / (drawCreateAccount,evalCreateAccount)  
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
, state<CreateAccountWaitForServer>   + on_entry<_>                               / setCreateAccountWaitForServer
, state<CreateAccountWaitForServer>   + event<shared_class::CreateAccountError>   / setErrorEvent                                                  
, state<CreateAccountWaitForServer>   + event<shared_class::LoginAccountSuccess>  / (setAccountName,process(makeGameMachine{}))         = X
, state<CreateAccountWaitForServer>   + event<createAccount>                                                                            = state<CreateAccount>
, state<CreateAccountWaitForServer>   + event<draw>                               / (drawCreateAccount,evalCreateAccountWaitForServer)
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
);
    // clang-format on
  }
};

#endif /* C9747240_569B_4FC4_A287_AFBC41ECAF3C */
