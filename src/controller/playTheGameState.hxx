#ifndef C162DA1A_2CC6_4528_9DE2_06DFB5B0B709
#define C162DA1A_2CC6_4528_9DE2_06DFB5B0B709

#include <cstddef>
#include <durak/gameData.hxx>
#include <game_01_shared_class/serialization.hxx>
struct Game
{
  durak::GameData gameData{};
  shared_class::DurakAllowedMoves allowedMoves{};
  shared_class::DurakTimers timers{};
  std::vector<bool> selectedCards{};
  boost::optional<size_t> selectedCardFromTable{};
  bool placeSelectedCardsOnTable{};
  bool surrender{};
  bool pass{};
};

struct game
{
};

struct goToLobby
{
};

struct leaveGame
{
};

struct gameWaitForServer
{
};

struct GameWaitForServer
{
};

#endif /* C162DA1A_2CC6_4528_9DE2_06DFB5B0B709 */
