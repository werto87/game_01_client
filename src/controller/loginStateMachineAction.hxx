#ifndef BD7A907D_9917_4122_9B87_77B2D778F12D
#define BD7A907D_9917_4122_9B87_77B2D778F12D

#include "src/controller/loginStateMachineState.hxx"
#include "src/controller/sendObject.hxx"
#include "src/controller/stateMachineData.hxx"
#include <boost/sml.hpp>
#include <chrono>
#include <game_01_shared_class/serialization.hxx>
#include <optional>
#include <string>
#include <vector>

namespace sml = boost::sml;

auto const showWantToRelog = [] (shared_class::WantToRelog const &wantToRelog, MessageBoxPopup &messageBoxPopup) {
  messageBoxPopup.event = wantToRelog;
  messageBoxPopup.buttons = std::vector<Button>{ { .name = "Lobby", .pressed = false, .disabled = false }, { .name = "Back to Create Game Lobby", .pressed = false, .disabled = false } };
  messageBoxPopup.message = "Do you want to go back to " + wantToRelog.destination;
};

auto const evalLogin = [] (Login &login, MessagesToSendToServer &messagesToSendToServer, sml::back::process<loginWaitForServer, createAccount> process_event) {
  if (login.loginClicked && not login.accountName.empty () && not login.password.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LoginAccount{ .accountName = login.accountName, .password = login.password });
      process_event (loginWaitForServer{});
    }
  if (login.createAccountClicked) process_event (createAccount{});
};

auto const evalLoginWaitForServer = [] (std::optional<WaitForServer> &waitForServer, MessagesToSendToServer &messagesToSendToServer, MessageBoxPopup &messageBoxPopup, sml::back::process<login, shared_class::LoginAccountSuccess> process_event) {
  if (std::holds_alternative<shared_class::WantToRelog> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed)
        {
          messageBoxPopup.buttons.front ().disabled = true;
          messageBoxPopup.buttons.back ().disabled = true;
          process_event (shared_class::LoginAccountSuccess{});
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::RelogTo{ .wantsToRelog = false });
        }
      else if (messageBoxPopup.buttons.back ().pressed)
        {
          messageBoxPopup.buttons.front ().disabled = true;
          messageBoxPopup.buttons.back ().disabled = true;
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::RelogTo{ .wantsToRelog = true });
        }
    }
  else if (std::holds_alternative<shared_class::LoginAccountError> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed)
        {
          process_event (login{});
        }
    }
  else
    {
      if (waitForServer->buttons.front ().pressed)
        {
          process_event (login{});
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LoginAccountCancel{});
        }
    }
};

auto const evalCreateAccount = [] (CreateAccount &createAccount, MessagesToSendToServer &messagesToSendToServer, sml::back::process<createAccountWaitForServer, login> process_event) {
  if (createAccount.createAccountClicked && not createAccount.accountName.empty () && not createAccount.password.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::CreateAccount{ .accountName = createAccount.accountName, .password = createAccount.password });
      process_event (createAccountWaitForServer{});
    }
  if (createAccount.backToLoginClicked) process_event (login{});
};

auto const evalCreateAccountWaitForServer = [] (std::optional<WaitForServer> &waitForServer, MessageBoxPopup &messageBoxPopup, MessagesToSendToServer &messagesToSendToServer, sml::back::process<createAccount> process_event) {
  if (std::holds_alternative<shared_class::CreateAccountError> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed)
        {
          process_event (createAccount{});
        }
    }
  else
    {
      if (waitForServer && waitForServer->buttons.front ().pressed)
        {
          process_event (createAccount{});
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::CreateAccountCancel{});
        }
    }
};

auto const setAccountName = [] (auto const &loginSuccessEv, MakeGameMachineData &makeGameMachineData) { makeGameMachineData.accountName = loginSuccessEv.accountName; };

auto const setLoginWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = WaitForServer{};
  using timer = std::chrono::system_clock;
  waitForServer->buttons = std::vector<Button>{ { .name = "Cancel Sign in", .pressed = false, .disabled = false } };
  waitForServer->clock_wait = timer::now ();
};
auto const setCreateAccountWaitForServer = [] (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer) {
  messageBoxPopup = MessageBoxPopup{};
  waitForServer = WaitForServer{};
  using timer = std::chrono::system_clock;
  waitForServer->buttons = std::vector<Button>{ { .name = "Cancel Create Account", .pressed = false, .disabled = false } };
  waitForServer->clock_wait = timer::now ();
};

#endif /* BD7A907D_9917_4122_9B87_77B2D778F12D */
