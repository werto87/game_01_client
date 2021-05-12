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

const auto showWantToRelog = [] (shared_class::WantToRelog const &wantToRelog, MessageBoxPopup &messageBoxPopup) {
  messageBoxPopup.event = wantToRelog;
  messageBoxPopup.buttons = std::vector<Button>{ { "Lobby", false }, { "Back to Create Game Lobby", false } };
  messageBoxPopup.message = "Do you want to go back to " + wantToRelog.destination;
};

const auto evalLogin = [] (Login &login, MessagesToSendToServer &messagesToSendToServer, sml::back::process<loginWaitForServer, createAccount> process_event) {
  if (login.loginClicked && not login.accountName.empty () && not login.password.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LoginAccount{ .accountName = login.accountName, .password = login.password });
      process_event (loginWaitForServer{});
    }
  if (login.createAccountClicked) process_event (createAccount{});
};

const auto evalLoginWaitForServer = [] (LoginWaitForServer &loginWaitForServer, MessagesToSendToServer &messagesToSendToServer, MessageBoxPopup &messageBoxPopup, sml::back::process<login, shared_class::LoginAccountSuccess> process_event) {
  if (std::holds_alternative<shared_class::WantToRelog> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed)
        {
          process_event (shared_class::LoginAccountSuccess{});
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::RelogTo{ .wantsToRelog = false });
        }
      else if (messageBoxPopup.buttons.back ().pressed)
        {
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
      if (loginWaitForServer.buttons.front ().second)
        {
          process_event (login{});
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LoginAccountCancel{});
        }
    }
};

const auto evalLoginError = [] (LoginError &loginError, sml::back::process<login> process_event) {
  if (loginError.backToLoginClicked) process_event (login{});
};

const auto evalCreateAccount = [] (CreateAccount &createAccount, MessagesToSendToServer &messagesToSendToServer, sml::back::process<createAccountWaitForServer, login> process_event) {
  if (createAccount.createAccountClicked && not createAccount.accountName.empty () && not createAccount.password.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::CreateAccount{ .accountName = createAccount.accountName, .password = createAccount.password });
      process_event (createAccountWaitForServer{});
    }
  if (createAccount.backToLoginClicked) process_event (login{});
};

const auto evalCreateAccountWaitForServer = [] (MessageBoxPopup &messageBoxPopup, MessagesToSendToServer &messagesToSendToServer, sml::back::process<createAccount> process_event) {
  if (std::holds_alternative<shared_class::CreateAccountError> (messageBoxPopup.event))
    {
      if (messageBoxPopup.buttons.front ().pressed)
        {
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::CreateAccountCancel{});
          process_event (createAccount{});
        }
    }
};

const auto evalCreateAccountError = [] (CreateAccountError &createAccountError, sml::back::process<createAccount> process_event) {
  if (createAccountError.backToAccountClicked) process_event (createAccount{});
};
const auto evalCreateAccountSuccess = [] (CreateAccountSuccess &createAccountSuccess, sml::back::process<createAccount> process_event) {
  if (createAccountSuccess.backToAccountClicked) process_event (createAccount{});
};

const auto setAccountName = [] (auto const &loginSuccessEv, MakeGameMachineData &makeGameMachineData) { makeGameMachineData.accountName = loginSuccessEv.accountName; };

const auto reactToJoinChannelSuccess = [] (shared_class::JoinChannelSuccess const &joinChannelSuccess, MakeGameMachineData &makeGameMachineData) { makeGameMachineData.chatData.channelMessages.insert_or_assign (joinChannelSuccess.channel, std::vector<std::string>{}); };
const auto reactToMessage = [] (shared_class::Message const &message, MakeGameMachineData &makeGameMachineData) {
  if (makeGameMachineData.chatData.channelMessages.count (message.channel) == 1)
    {
      makeGameMachineData.chatData.channelMessages.at (message.channel).push_back (message.fromAccount + ": " + message.message);
    }
};

const auto setLogin = [] (Login &login, MessageBoxPopup &messageBoxPopup) {
  login = Login{};
  messageBoxPopup = MessageBoxPopup{};
};
const auto setCreateAccount = [] (CreateAccount &createAccount, MessageBoxPopup &messageBoxPopup) {
  createAccount = CreateAccount{};
  messageBoxPopup = MessageBoxPopup{};
};

const auto setLoginWaitForServer = [] (LoginWaitForServer &loginWaitForServer, MessageBoxPopup &messageBoxPopup) {
  messageBoxPopup = MessageBoxPopup{};
  loginWaitForServer = LoginWaitForServer{};
  using timer = std::chrono::system_clock;
  loginWaitForServer.clock_wait = timer::now ();
};
const auto setCreateAccountWaitForServer = [] (CreateAccountWaitForServer &createAccountWaitForServer, MessageBoxPopup &messageBoxPopup) {
  messageBoxPopup = MessageBoxPopup{};
  createAccountWaitForServer = CreateAccountWaitForServer{};
  using timer = std::chrono::system_clock;
  createAccountWaitForServer.clock_wait = timer::now ();
};

#endif /* BD7A907D_9917_4122_9B87_77B2D778F12D */
