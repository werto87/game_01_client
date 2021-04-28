#include "src/controller/webservice.hxx"
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
#include <boost/type_index.hpp>
#include <confu_boost/confuBoost.hxx>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <exception>
#include <game_01_shared_class/serialization.hxx>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::deque<std::string> WebserviceController::msgToSend{};
Session WebserviceController::session{};

void
WebserviceController::createAccountSuccess (std::string const &objectAsString)
{
  auto createAccountSuccessObject = confu_boost::toObject<shared_class::CreateAccountSuccess> (objectAsString);
  upsertAccount (database::Account{ .id = createAccountSuccessObject.accountId, .accountName = createAccountSuccessObject.accountName, .password = {} });
}

void
WebserviceController::createAccountError (std::string const &objectAsString)
{
  auto createAccountErrorObject = confu_boost::toObject<shared_class::CreateAccountError> (objectAsString);
  session.createAccountErrorMessage = createAccountErrorObject.error;
}

void
WebserviceController::loginAccountSuccess (std::string const &objectAsString)
{
  auto loginAccountSuccessObject = confu_boost::toObject<shared_class::LoginAccountSuccess> (objectAsString);
  session.isLoggedIn = true;
}

void
WebserviceController::loginAccountError (std::string const &objectAsString)
{
  auto loginAccountErrorObject = confu_boost::toObject<shared_class::LoginAccountError> (objectAsString);
  session.isLoggedIn = false;
  session.loggInMessageFromServer = loginAccountErrorObject.error;
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
