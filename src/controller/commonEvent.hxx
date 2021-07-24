#ifndef C5C45508_B6C7_4102_BF47_8EE62A24216E
#define C5C45508_B6C7_4102_BF47_8EE62A24216E

#include "src/controller/makeGameMachineState.hxx"
#include "src/controller/stateMachineData.hxx"

class ImFont;

struct draw
{
  float windowSizeX{};
  float windowSizeY{};
  ImFont *biggerFont{};
};

auto const setErrorEvent = [] (auto const &error, MessageBoxPopup &messageBoxPopup) {
  messageBoxPopup.event = error;
  messageBoxPopup.message = error.error;
  messageBoxPopup.buttons = { { .name = "Back", .pressed = false } };
};

auto const resetPopupAndWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, CreateGameLobby &createGameLobby) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = std::optional<WaitForServer>{};
};

auto const setMessageBoxPopupAndSetWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = WaitForServer{};
  using timer = std::chrono::system_clock;
  waitForServer->clock_wait = timer::now ();
};

#endif /* C5C45508_B6C7_4102_BF47_8EE62A24216E */
