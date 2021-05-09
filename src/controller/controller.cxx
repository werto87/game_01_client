#include "src/controller/controller.hxx"
#include "src/controller/database.hxx"
#include "src/model/database.hxx"
#include <algorithm>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/type_index.hpp>
#include <confu_boost/confuBoost.hxx>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <exception>
#include <game_01_shared_class/serialization.hxx>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

std::deque<std::string> WebserviceController::msgToSend{};
Session WebserviceController::session{};

// TODO move this logic so it does send events to state machine

void
WebserviceController::createAccountSuccess (std::string const &objectAsString)
{
  auto createAccountSuccessObject = confu_boost::toObject<shared_class::CreateAccountSuccess> (objectAsString);
  upsertAccount (database::Account{ .accountName = createAccountSuccessObject.accountName, .password = {} });
  session.isAccountCreateSuccess = true;
}

void
WebserviceController::createAccountError (std::string const &objectAsString)
{
  auto createAccountErrorObject = confu_boost::toObject<shared_class::CreateAccountError> (objectAsString);
  session.createAccountErrorMessage = createAccountErrorObject.error;
  session.isAccountCreateSuccess = false;
}

void
WebserviceController::loginAccountSuccess (std::string const &objectAsString)
{
  auto loginAccountSuccessObject = confu_boost::toObject<shared_class::LoginAccountSuccess> (objectAsString);
  session.isLoggedIn = true;
  session.accountName = loginAccountSuccessObject.accountName;
}

void
WebserviceController::loginAccountError (std::string const &objectAsString)
{
  auto loginAccountErrorObject = confu_boost::toObject<shared_class::LoginAccountError> (objectAsString);
  session.isLoggedIn = false;
  session.loggInMessageFromServer = loginAccountErrorObject.error;
}

void
WebserviceController::logoutAccountSuccess (std::string const &)
{
  session = {};
}

void
WebserviceController::joinChannelSuccess (std::string const &objectAsString)
{
  auto joinChannelSuccessObject = confu_boost::toObject<shared_class::JoinChannelSuccess> (objectAsString);
  session.channelMessages.insert_or_assign (joinChannelSuccessObject.channel, std::vector<std::string>{});
}

void
WebserviceController::joinChannelError (std::string const &objectAsString)
{
  auto joinChannelErrorObject = confu_boost::toObject<shared_class::JoinChannelError> (objectAsString);
  std::terminate ();
}

void
WebserviceController::message (std::string const &objectAsString)
{
  auto messageObject = confu_boost::toObject<shared_class::Message> (objectAsString);
  auto messagesItr = session.channelMessages.find (messageObject.channel);
  if (messagesItr != session.channelMessages.end ())
    {
      messagesItr->second.push_back (messageObject.fromAccount + ": " + messageObject.message);
    }
}

void
WebserviceController::broadCastMessageSuccess (std::string const &objectAsString)
{
  // auto broadCastMessageSuccessObject = confu_boost::toObject<shared_class::BroadCastMessageSuccess> (objectAsString);
  // std::terminate ();
}

void
WebserviceController::broadCastMessageError (std::string const &objectAsString)
{
  // auto broadCastMessageErrorObject = confu_boost::toObject<shared_class::BroadCastMessageError> (objectAsString);
  // std::terminate ();
}

void
WebserviceController::createGameLobbySuccess (std::string const &objectAsString)
{
  std::terminate ();
}

void
WebserviceController::createGameLobbyError (std::string const &objectAsString)
{
  std::terminate ();
}

void
WebserviceController::joinGameLobbySuccess (std::string const &objectAsString)
{
  std::terminate ();
}

void
WebserviceController::joinGameLobbyError (std::string const &objectAsString)
{
  std::terminate ();
}

void
WebserviceController::usersInGameLobby (std::string const &objectAsString)
{
  auto usersInGameLobbyObject = confu_boost::toObject<shared_class::UsersInGameLobby> (objectAsString);
  session.accountNamesInGameLobby.clear ();
  session.gameLobbyName = usersInGameLobbyObject.name;
  session.maxUserInGameLobby = usersInGameLobbyObject.maxUserSize;
  std::ranges::transform (usersInGameLobbyObject.users, std::back_inserter (session.accountNamesInGameLobby), [] (auto const &user) { return user.accountName; });
}

void
WebserviceController::maxUserSizeInCreateGameLobby (std::string const &objectAsString)
{
  auto maxUserSizeInCreateGameLobbyObject = confu_boost::toObject<shared_class::MaxUserSizeInCreateGameLobby> (objectAsString);
  session.maxUserInGameLobby = maxUserSizeInCreateGameLobbyObject.maxUserSize;
}

void
WebserviceController::setMaxUserSizeInCreateGameLobbyError (std::string const &objectAsString)
{
  auto setMaxUserSizeInCreateGameLobbyErrorObject = confu_boost::toObject<shared_class::SetMaxUserSizeInCreateGameLobbyError> (objectAsString);
  std::terminate ();
}

void
WebserviceController::leaveGameLobbySuccess (std::string const &objectAsString)
{
  session.gameLobbyName = {};
  session.maxUserInGameLobby = {};
  session.accountNamesInGameLobby = {};
}

void
WebserviceController::leaveGameLobbyError (std::string const &objectAsString)
{
}

void
WebserviceController::wantToRelog (std::string const &objectAsString)
{
  auto wantToRelogObject = confu_boost::toObject<shared_class::WantToRelog> (objectAsString);
  session.isLoggedIn = true;
  session.accountName = wantToRelogObject.accountName;
  session.relogToDestination = wantToRelogObject.destination;
}

void
WebserviceController::relogToError (std::string const &objectAsString)
{
  auto relogToErrorObject = confu_boost::toObject<shared_class::RelogToError> (objectAsString);
  session.relogToError = relogToErrorObject.error;
}

std::vector<std::string>
WebserviceController::handleMessage (std::string const &msg)
{
  auto result = std::vector<std::string>{};
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () == 2)
    {
      auto typeToSearch = splitMesssage.at (0);
      auto objectAsString = splitMesssage.at (1);
      if (typeToSearch == "CreateAccountSuccess")
        {
          createAccountSuccess (objectAsString);
        }
      else if (typeToSearch == "CreateAccountError")
        {
          createAccountError (objectAsString);
        }
      else if (typeToSearch == "LoginAccountSuccess")
        {
          loginAccountSuccess (objectAsString);
        }
      else if (typeToSearch == "LoginAccountError")
        {
          loginAccountError (objectAsString);
        }
      else if (typeToSearch == "LogoutAccountSuccess")
        {
          logoutAccountSuccess (objectAsString);
        }
      else if (typeToSearch == "JoinChannelSuccess")
        {
          joinChannelSuccess (objectAsString);
        }
      else if (typeToSearch == "JoinChannelError")
        {
          joinChannelError (objectAsString);
        }
      else if (typeToSearch == "Message")
        {
          message (objectAsString);
        }
      else if (typeToSearch == "BroadCastMessageSuccess")
        {
          broadCastMessageSuccess (objectAsString);
        }
      else if (typeToSearch == "BroadCastMessageError")
        {
          broadCastMessageError (objectAsString);
        }
      else if (typeToSearch == "CreateGameLobbySuccess")
        {
          createGameLobbySuccess (objectAsString);
        }
      else if (typeToSearch == "CreateGameLobbyError")
        {
          createGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "JoinGameLobbySuccess")
        {
          joinGameLobbySuccess (objectAsString);
        }
      else if (typeToSearch == "JoinGameLobbyError")
        {
          joinGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "UsersInGameLobby")
        {
          usersInGameLobby (objectAsString);
        }
      else if (typeToSearch == "MaxUserSizeInCreateGameLobby")
        {
          maxUserSizeInCreateGameLobby (objectAsString);
        }
      else if (typeToSearch == "SetMaxUserSizeInCreateGameLobbyError")
        {
          setMaxUserSizeInCreateGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "LeaveGameLobbySuccess")
        {
          leaveGameLobbySuccess (objectAsString);
        }
      else if (typeToSearch == "LeaveGameLobbyError")
        {
          leaveGameLobbyError (objectAsString);
        }
      else if (typeToSearch == "WantToRelog")
        {
          wantToRelog (objectAsString);
        }
      else
        {
          std::cout << "could not find a match for typeToSearch '" << typeToSearch << "'" << std::endl;
        }
    }
  return result;
}

void
WebserviceController::sendMessage (std::string const &msg)
{
  msgToSend.push_back (msg);
}

void
WebserviceController::sendMessage (std::vector<std::string> &&msgs)
{
  msgToSend.insert (msgToSend.end (), make_move_iterator (msgs.begin ()), make_move_iterator (msgs.end ()));
}

bool
WebserviceController::messageEmpty ()
{
  return msgToSend.empty ();
}

std::string
WebserviceController::popFront ()
{
  auto result = std::string{};
  if (msgToSend.empty ())
    {
      throw std::out_of_range ("cant pop from empty deque");
    }
  else
    {
      result = std::move (msgToSend.front ());
      msgToSend.pop_front ();
    }
  return result;
}
bool
WebserviceController::hasLoginState ()
{
  return session.isLoggedIn.has_value ();
}

bool
WebserviceController::isLoggedIn ()
{
  return session.isLoggedIn.value ();
}

bool
WebserviceController::hasAccountCreateSuccessState ()
{
  return session.isAccountCreateSuccess.has_value ();
}

bool
WebserviceController::isAccountCreateSuccess ()
{
  return session.isAccountCreateSuccess.value ();
}

void
WebserviceController::resetSession ()
{
  session = Session{};
}

std::string
WebserviceController::logInMessageFromServer ()
{
  return session.loggInMessageFromServer.value ();
}

bool
WebserviceController::isCreateAccountError ()
{
  return session.createAccountErrorMessage.has_value ();
}

std::string
WebserviceController::createAccountError ()
{
  return session.createAccountErrorMessage.value ();
}

size_t
WebserviceController::channelCount ()
{
  return session.channelMessages.size ();
}

std::vector<std::string>
WebserviceController::channelNames ()
{
  auto result = std::vector<std::string>{};
  session.channelMessages >>= pipes::transform ([] (auto const &channelAndMessages) { return std::get<0> (channelAndMessages); }) >>= pipes::push_back (result);
  return result;
}

std::vector<std::string> const &
WebserviceController::messagesForChannel (std::string const &channel)
{
  return session.channelMessages.at (channel);
}

bool
WebserviceController::hasCreateGameLobbyName ()
{
  return session.gameLobbyName.has_value ();
}

std::string
WebserviceController::createGameLobbyName ()
{
  return session.gameLobbyName.value ();
}

std::vector<std::string> const &
WebserviceController::accountNamesInCreateGameLobby ()
{
  return session.accountNamesInGameLobby;
}

size_t
WebserviceController::getMaxUsersInGameLobby ()
{
  return session.maxUserInGameLobby;
}

std::string
WebserviceController::getAccountName ()
{
  return session.accountName;
}

bool
WebserviceController::hasRelogToError ()
{
  return session.relogToError.has_value ();
}

std::string
WebserviceController::relogToError ()
{
  return session.relogToError.value ();
}

void
WebserviceController::removeRelogToError ()
{
  session.relogToError = {};
}

bool
WebserviceController::hasRelogToDestination ()
{
  return session.relogToDestination.has_value ();
}

std::string
WebserviceController::relogToDestination ()
{
  return session.relogToDestination.value ();
}

void
WebserviceController::removeRelogToDestinationMessage ()
{
  session.relogToDestination = {};
}

void
WebserviceController::removeCreateAccountErrorMessage ()
{
  session.createAccountErrorMessage = {};
}

void
WebserviceController::removeIsAccountCreateSuccess ()
{
  session.isAccountCreateSuccess = {};
}
