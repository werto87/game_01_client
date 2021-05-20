#ifndef BA8536A2_B2AB_41EA_B13C_E0CDE40F8344
#define BA8536A2_B2AB_41EA_B13C_E0CDE40F8344

#include "src/controller/playTheGameState.hxx"
#include "src/controller/stateMachineData.hxx"
#include <bits/ranges_algo.h>
#include <durak/gameData.hxx>
#include <game_01_shared_class/serialization.hxx>
#include <numeric>
#include <pipes/filter.hpp>
#include <pipes/mux.hpp>
#include <pipes/push_back.hpp>
#include <pipes/transform.hpp>

auto const setGameData = [] (durak::GameData const &gameDataEv, Game &game, MakeGameMachineData &makeGameMachineData) {
  game.gameData = gameDataEv;
  if (auto player = std::ranges::find_if (gameDataEv.players, [&] (auto const &_player) { return _player.name == makeGameMachineData.accountName; }); player != gameDataEv.players.end ())
    if (game.selectedCards.size () != player->cards.size ())
      {
        game.selectedCards = std::vector<bool> (player->cards.size ());
      }
};

auto const setDurakDefendWantsToTakeCardsFromTableDoYouWantToAddCardsEvent = [] (MessageBoxPopup &messageBoxPopup, auto const &event) {
  messageBoxPopup.event = event;
  messageBoxPopup.message = "Defend wants to take Cards. You can add cards. Pass if you are done";
  messageBoxPopup.buttons = { { .name = "Back" } };
};

auto const setDurakAskDefendWantToTakeCards = [] (MessageBoxPopup &messageBoxPopup, auto const &event) {
  messageBoxPopup.event = event;
  messageBoxPopup.message = "Attack is over do you want to Take Cards from Table or discard them and Attack";
  messageBoxPopup.buttons = { { .name = "Take Cards From Table" } };
  messageBoxPopup.buttons.emplace_back (Button{ .name = "Discard Cards from Table and Attack" });
};

auto const evalGame = [] (MessageBoxPopup &messageBoxPopup, Game &game, MakeGameMachineData &makeGameMachineData, MessagesToSendToServer &messagesToSendToServer, sml::back::process<gameWaitForServer> process_event) {
  auto &chatData = makeGameMachineData.chatData;
  if (chatData.joinChannelClicked && not chatData.channelToJoin.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::JoinChannel{ .channel = chatData.channelToJoin });
      chatData.channelToJoin.clear ();
      process_event (gameWaitForServer{});
    }
  else if (chatData.sendMessageClicked && chatData.selectedChannelName && not chatData.selectedChannelName->empty () && not chatData.messageToSendToChannel.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::BroadCastMessage{ .channel = chatData.selectedChannelName.value (), .message = chatData.messageToSendToChannel });
      chatData.messageToSendToChannel.clear ();
      process_event (gameWaitForServer{});
    }
  else if (game.placeSelectedCardsOnTable)
    {
      if (auto player = std::ranges::find_if (game.gameData.players, [accountName = makeGameMachineData.accountName] (durak::PlayerData const &_player) { return accountName == _player.name; }); player != game.gameData.players.end ())
        {
          if (player->playerRole == durak::PlayerRole::attack || player->playerRole == durak::PlayerRole::assistAttacker)
            {
              auto durakAttack = shared_class::DurakAttack{};
              pipes::mux (game.selectedCards, player->cards) >>= pipes::filter ([] (bool enabled, boost::optional<durak::Card> const &card) { return enabled && card; }) >>= pipes::transform ([] (bool, boost::optional<durak::Card> const &card) { return card.value (); }) >>= pipes::push_back (durakAttack.cards);
              if (not durakAttack.cards.empty ())
                {
                  process_event (gameWaitForServer{});
                  sendObject (messagesToSendToServer.messagesToSendToServer, durakAttack);
                }
            }
          else if (player->playerRole == durak::PlayerRole::defend)
            {
              if (game.selectedCardFromTable && game.selectedCardFromTable.value () < game.gameData.table.size ())
                {
                  if (auto tableCardPair = game.gameData.table.at (game.selectedCardFromTable.value ()); not tableCardPair.second)
                    {
                      durak::Card cardToBeat = tableCardPair.first;
                      auto selectedCards = std::vector<durak::Card>{};
                      pipes::mux (game.selectedCards, player->cards) >>= pipes::filter ([] (bool enabled, boost::optional<durak::Card> const &card) { return enabled && card; }) >>= pipes::transform ([] (bool, boost::optional<durak::Card> const &card) { return card.value (); }) >>= pipes::push_back (selectedCards);
                      if (selectedCards.size () == 1)
                        {
                          process_event (gameWaitForServer{});
                          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::DurakDefend{ .cardToBeat = cardToBeat, .card = selectedCards.front () });
                        }
                    }
                }
            }
        }
    }
  else if (game.pass)
    {
      if (auto player = std::ranges::find_if (game.gameData.players, [accountName = makeGameMachineData.accountName] (durak::PlayerData const &_player) { return accountName == _player.name; }); player != game.gameData.players.end ())
        {
          if (player->playerRole == durak::PlayerRole::defend)
            {
              process_event (gameWaitForServer{});
              sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::DurakDefendPass{});
            }
          else if (player->playerRole == durak::PlayerRole::assistAttacker)
            {
              process_event (gameWaitForServer{});
              sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::DurakAssistPass{});
            }
          else if (player->playerRole == durak::PlayerRole::attack)
            {
              process_event (gameWaitForServer{});
              sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::DurakAttackPass{});
            }
        }
    }
  else if (std::holds_alternative<shared_class::DurakDefendWantsToTakeCardsFromTableDoYouWantToAddCards> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed)
        {
          messageBoxPopup = MessageBoxPopup{};
        }
    }
  else if (std::holds_alternative<shared_class::DurakAskDefendWantToTakeCards> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed)
        {
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::DurakAskDefendWantToTakeCardsAnswer{ .answer = true });
          messageBoxPopup = MessageBoxPopup{};
        }
      else if (messageBoxPopup.buttons.at (1).pressed)
        {
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::DurakAskDefendWantToTakeCardsAnswer{ .answer = false });
          messageBoxPopup = MessageBoxPopup{};
        }
    }
};

auto const evalGameWaitForServer = [] (MessageBoxPopup &messageBoxPopup, MessagesToSendToServer &, sml::back::process<game> process_event) {
  if (std::holds_alternative<shared_class::DurakAttackError> (messageBoxPopup.event) || std::holds_alternative<shared_class::DurakDefendError> (messageBoxPopup.event) || std::holds_alternative<shared_class::DurakDefendPassError> (messageBoxPopup.event) || std::holds_alternative<shared_class::DurakDefendWantsToTakeCardsFromTableDoYouWantToAddCards> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed) process_event (game{});
    }
};

#endif /* BA8536A2_B2AB_41EA_B13C_E0CDE40F8344 */