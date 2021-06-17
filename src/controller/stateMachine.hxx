
#ifndef F74E399C_E6DE_488D_B216_CC0117DE1B1D
#define F74E399C_E6DE_488D_B216_CC0117DE1B1D

#include "src/controller/loginStateMachine.hxx"
#include "src/controller/makeGameMachine.hxx"
#include "src/controller/playTheGame.hxx"
#include <boost/sml.hpp>
#include <queue>

const auto reset = [] (Lobby &lobby, Login &login, CreateAccount &createAccount, Game &game) {
  lobby = Lobby{};
  login = Login{};
  createAccount = CreateAccount{};
  game = Game{};
};
const auto resetGameMachineData = [] (MakeGameMachineData &makeGameMachineData) { makeGameMachineData = MakeGameMachineData{}; };
struct WrapperMachine
{
  auto
  operator() ()
  {
    using namespace sml;
    return make_transition_table (
        // clang-format off
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<LoginStateMachine>  + event<makeGameMachine>                                                                                                    = state<MakeGameMachine>
, state<MakeGameMachine>    + event<shared_class::LogoutAccountSuccess> / (reset, resetGameMachineData)                                                 = state<LoginStateMachine>
, state<LoginStateMachine>  + event<goToCreateGameLobby>                                                                                                = state<MakeGameMachine>
, state<MakeGameMachine>    + sml::on_entry<goToCreateGameLobby>        / (process(lobbyWaitForServer{}),process(shared_class::JoinGameLobbySuccess{}))
, state<MakeGameMachine>    + event<startGame>                                                                                                          = state<PlayTheGame>
, state<PlayTheGame>        + sml::on_entry<_>                          / reset
, state<PlayTheGame>        + event<goToLobby>                                                                                                          = state<MakeGameMachine>
,*"error_handler"_s         + unexpected_event<_>                       / [](auto const& event){std::cout<<"unhandled event: '"<<typeNameWithOutNamespace(event)<<"'"<<std::endl;}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
// clang-format on   
    );
  }
};


struct my_logger
{
  template <class SM, class TEvent>
  void
  log_process_event (const TEvent &)
  {
    // if(std::string{sml::aux::get_type_name<TEvent> ()}!=std::string{"draw"}){
    //   printf ("[%s][process_event] %s\n", sml::aux::get_type_name<SM> (), sml::aux::get_type_name<TEvent> ());
    // }
  }

  template <class SM, class TGuard, class TEvent>
  void
  log_guard (const TGuard &, const TEvent &, bool /*result*/)
  {
    // printf ("[%s][guard] %s %s %s\n", sml::aux::get_type_name<SM> (), sml::aux::get_type_name<TGuard> (), sml::aux::get_type_name<TEvent> (), (result ? "[OK]" : "[Reject]"));
  }

  template <class SM, class TAction, class TEvent>
  void
  log_action (const TAction &, const TEvent &)
  {
    // printf ("[%s][action] %s %s\n", sml::aux::get_type_name<SM> (), sml::aux::get_type_name<TAction> (), sml::aux::get_type_name<TEvent> ());
  }

  template <class SM, class TSrcState, class TDstState>
  void
  log_state_change (const TSrcState &src, const TDstState &dst)
  {
    //printf ("[%s] %s -> %s\n", sml::aux::get_type_name<SM> (), src.c_str (), dst.c_str ());

    std::cout<<"["<< sml::aux::get_type_name<SM> ()<<"] "<<src.c_str ()<<" -> "<<dst.c_str ()<<std::endl;
  }
};

typedef sml::sm<WrapperMachine, sml::logger<my_logger>, sml::process_queue<std::queue>> StateMachine;


#endif /* F74E399C_E6DE_488D_B216_CC0117DE1B1D */
