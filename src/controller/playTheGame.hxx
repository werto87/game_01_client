#ifndef F34E56B1_0E5B_4134_88EB_EA1AB782EBCD
#define F34E56B1_0E5B_4134_88EB_EA1AB782EBCD

#include "src/controller/commonEvent.hxx"
#include "src/controller/makeGameMachineAction.hxx"
#include "src/controller/playTheGameAction.hxx"
#include "src/ui/screen.hxx"
#include <game_01_shared_class/serialization.hxx>

struct PlayTheGame
{
  auto
  operator() () const noexcept
  {
    // TODO it is possible that a message is recived while we are waiting for server for another message
    using namespace sml;
    return make_transition_table (
        // clang-format off
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/      
* state<Game>                         + on_entry<_>                               / resetPopupAndWaitForServer
, state<Game>                         + event<gameWaitForServer>                                                                                          = state<GameWaitForServer>
, state<Game>                         + event<durak::GameData>                    / setGameData
, state<Game>                         + event<draw>                               / (drawGame,evalGame)         
, state<Game>                         + event<shared_class::Message>              / reactToMessage        
, state<Game>                         + event<shared_class::DurakDefendWantsToTakeCardsFromTableDoYouWantToAddCards>     / setDurakDefendWantsToTakeCardsFromTableDoYouWantToAddCardsEvent
, state<Game>                         + event<shared_class::DurakAskDefendWantToTakeCards>                               / setDurakAskDefendWantToTakeCards

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
, state<GameWaitForServer>            + on_entry<_>                               / setMessageBoxPopupAndSetWaitForServer
, state<GameWaitForServer>            + event<durak::GameData>                    / setGameData                                                                                         
, state<GameWaitForServer>            + event<game>                                                                                                       = state<Game>
, state<GameWaitForServer>            + event<shared_class::DurakAttackSuccess>                                                                           = state<Game>
, state<GameWaitForServer>            + event<shared_class::DurakDefendWantsToTakeCardsFromTableDoneAddingCardsSuccess>                                   = state<Game>
, state<GameWaitForServer>            + event<shared_class::DurakAttackPassSuccess>                                                                       = state<Game>
, state<GameWaitForServer>            + event<shared_class::DurakAttackError>     / setErrorEvent
, state<GameWaitForServer>            + event<shared_class::DurakDefendPassSuccess>                                                                       = state<Game>
, state<GameWaitForServer>            + event<shared_class::DurakDefendPassError>     / setErrorEvent
, state<GameWaitForServer>            + event<shared_class::DurakDefendSuccess>                                                                           = state<Game>
, state<GameWaitForServer>            + event<shared_class::DurakDefendError>     / setErrorEvent
, state<GameWaitForServer>            + event<shared_class::DurakAskDefendWantToTakeCardsAnswerError>     / setErrorEvent
, state<GameWaitForServer>            + event<draw>                               / (drawGame,evalGameWaitForServer)      
, state<GameWaitForServer>            + event<shared_class::JoinChannelSuccess>   / reactToJoinChannelSuccess                                             = state<Game>   
, state<GameWaitForServer>            + event<shared_class::Message>              / reactToMessage
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
);
    // clang-format on
  }
};

#endif /* F34E56B1_0E5B_4134_88EB_EA1AB782EBCD */
