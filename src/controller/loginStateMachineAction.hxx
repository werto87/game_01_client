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

const auto showLoginError = [] (auto const &loginAccountError, LoginWaitForServer &loginWaitForServer) {
  loginWaitForServer.buttons.at (0).first = "back";
  loginWaitForServer.message = loginAccountError.error;
};

const auto showWantToRelog = [] (shared_class::WantToRelog const &wantToRelog, LoginWaitForServer &loginWaitForServer) {
  loginWaitForServer.buttons = std::vector<std::pair<std::string, bool>>{ { "Lobby", false }, { "Back to Create Game Lobby", false } };
  loginWaitForServer.message = "Do you want to go back to " + wantToRelog.destination;
};

const auto evalLogin = [] (Login &login, MessagesToSendToServer &messagesToSendToServer, sml::back::process<loginWaitForServer, createAccount> process_event) {
  if (login.loginClicked && not login.accountName.empty () && not login.password.empty ())
    {
      sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::LoginAccount{ .accountName = login.accountName, .password = login.password });
      process_event (loginWaitForServer{});
    }
  if (login.createAccountClicked) process_event (createAccount{});
};

const auto evalLoginWaitForServer = [] (LoginWaitForServer &loginWaitForServer, MessagesToSendToServer &messagesToSendToServer, sml::back::process<login, shared_class::LoginAccountSuccess> process_event) {
  // TODO has only two buttons if want to relog is active
  // kinda complicated
  if (loginWaitForServer.buttons.size () == 2)
    {
      if (loginWaitForServer.buttons.front ().second)
        {
          process_event (shared_class::LoginAccountSuccess{});
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::RelogTo{ .wantsToRelog = false });
        }
      if (loginWaitForServer.buttons.at (1).second)
        {
          sendObject (messagesToSendToServer.messagesToSendToServer, shared_class::RelogTo{ .wantsToRelog = true });
        }
    }
  else
    {
      // TODO implement cancel should send something to the server
      if (loginWaitForServer.buttons.front ().second) process_event (login{});
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

const auto evalCreateAccountWaitForServer = [] (CreateAccountWaitForServer &createAccountWaitForServer, sml::back::process<createAccount> process_event) {
  // TODO implement cancel should send something to the server
  if (createAccountWaitForServer.buttons.front ().second) process_event (createAccount{});
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

const auto setLoginWaitForServer = [] (LoginWaitForServer &loginWaitForServer) {
  loginWaitForServer = LoginWaitForServer{};
  using timer = std::chrono::system_clock;
  loginWaitForServer.clock_wait = timer::now ();
};
const auto setCreateAccountWaitForServer = [] (CreateAccountWaitForServer &createAccountWaitForServer) {
  createAccountWaitForServer = CreateAccountWaitForServer{};
  using timer = std::chrono::system_clock;
  createAccountWaitForServer.clock_wait = timer::now ();
};

const auto showLoginErrorCreateAccount = [] (auto const &createAccountError, CreateAccountWaitForServer &createAccountWaitForServer) {
  createAccountWaitForServer.buttons = std::vector<std::pair<std::string, bool>>{ { "Back", false } };
  createAccountWaitForServer.message = createAccountError.error;
};

#endif /* BD7A907D_9917_4122_9B87_77B2D778F12D */
