#ifndef C5C45508_B6C7_4102_BF47_8EE62A24216E
#define C5C45508_B6C7_4102_BF47_8EE62A24216E

#include "src/controller/stateMachineData.hxx"

class ImFont;

struct draw
{
  float windowSizeX{};
  float windowSizeY{};
  ImFont *biggerFont{};
};

const auto setErrorEvent = [] (auto const &error, MessageBoxPopup &messageBoxPopup) {
  messageBoxPopup.event = error;
  messageBoxPopup.message = error.error;
  messageBoxPopup.buttons = { { .name = "Back", .pressed = false } };
};

const auto resetPopupAndWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = std::optional<WaitForServer>{};
};

#endif /* C5C45508_B6C7_4102_BF47_8EE62A24216E */
