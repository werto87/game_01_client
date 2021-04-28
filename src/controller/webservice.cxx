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
WebserviceController::createAccountSuccess (std::string const &msg)
{
  auto createAccountSuccessObject = confu_boost::toObject<shared_class::CreateAccountSuccess> (msg);
  upsertAccount (database::Account{ .id = createAccountSuccessObject.accountId, .accountName = createAccountSuccessObject.accountName, .password = {} });
}

void
WebserviceController::createAccountError (std::string const &msg)
{
  auto createAccountErrorObject = confu_boost::toObject<shared_class::CreateAccountError> (msg);
  std::terminate ();
}

void
WebserviceController::loginAccountSuccess (std::string const &msg)
{
  auto loginAccountSuccessObject = confu_boost::toObject<shared_class::LoginAccountSuccess> (msg);
  session.isLoggedIn = true;
}

void
WebserviceController::loginAccountError (std::string const &msg)
{
  auto loginAccountErrorObject = confu_boost::toObject<shared_class::LoginAccountError> (msg);
  session.isLoggedIn = false;
  session.loggInMessageFromServer = loginAccountErrorObject.error;
}

void
WebserviceController::joinChannelSuccess (std::string const &msg)
{
  auto joinChannelSuccessObject = confu_boost::toObject<shared_class::JoinChannelSuccess> (msg);
  std::terminate ();
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () == 2)
    {
      auto channel = confu_boost::toObject<shared_class::JoinChannel> (splitMesssage.at (1)).channel;
      session.channelMessages.insert_or_assign (channel, std::vector<std::string>{});
    }
}

void
WebserviceController::joinChannelError (std::string const &msg)
{
  auto joinChannelErrorObject = confu_boost::toObject<shared_class::JoinChannelError> (msg);
  std::terminate ();
}

void
WebserviceController::broadCastMessage (std::string const &msg)
{
  auto broadCastMessageObject = confu_boost::toObject<shared_class::BroadCastMessage> (msg);
  std::terminate ();
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () == 2)
    {
      boost::algorithm::split (splitMesssage, splitMesssage.at (1), boost::is_any_of (","));
      if (splitMesssage.size () == 2)
        {
          auto messagesItr = session.channelMessages.find (splitMesssage.at (0));
          if (messagesItr != session.channelMessages.end ())
            {
              messagesItr->second.push_back (splitMesssage.at (1));
            }
        }
    }
}

void
WebserviceController::broadCastMessageSuccess (std::string const &msg)
{
  auto broadCastMessageSuccessObject = confu_boost::toObject<shared_class::BroadCastMessageSuccess> (msg);
  std::terminate ();
}

void
WebserviceController::broadCastMessageError (std::string const &msg)
{
  auto broadCastMessageErrorObject = confu_boost::toObject<shared_class::BroadCastMessageError> (msg);
  std::terminate ();
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
      else if (typeToSearch == "BroadCastMessage")
        {
          broadCastMessage (objectAsString);
        }
      else if (typeToSearch == "BroadCastMessageSuccess")
        {
          broadCastMessageSuccess (objectAsString);
        }
      else if (typeToSearch == "BroadCastMessageError")
        {
          broadCastMessageError (objectAsString);
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
