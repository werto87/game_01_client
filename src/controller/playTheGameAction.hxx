#ifndef BA8536A2_B2AB_41EA_B13C_E0CDE40F8344
#define BA8536A2_B2AB_41EA_B13C_E0CDE40F8344

#include "src/controller/playTheGameState.hxx"
#include "src/controller/stateMachineData.hxx"
#include <bits/ranges_algo.h>
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

auto const evalGame = [] (Game &game, MakeGameMachineData &makeGameMachineData, MessagesToSendToServer &messagesToSendToServer, sml::back::process<gameWaitForServer> process_event) {
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
};

auto const evalGameWaitForServer = [] (MessageBoxPopup &messageBoxPopup, MessagesToSendToServer &, sml::back::process<game> process_event) {
  if (std::holds_alternative<shared_class::DurakAttackError> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed) process_event (game{});
    }
};

#endif /* BA8536A2_B2AB_41EA_B13C_E0CDE40F8344 */
