#include "src/controller/webservice.hxx"
#include "confu_soci/convenienceFunctionForSoci.hxx"
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
#include <exception>
#include <game_01_shared_class/serialization.hxx>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::deque<std::string> WebserviceController::msgToSend{};
Session WebserviceController::session{};

void
createAccount (std::string const &msg)
{
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () >= 2)
    {
      auto accountStringStream = std::stringstream{};
      accountStringStream << splitMesssage.at (1);
      boost::archive::text_iarchive ia (accountStringStream);
      auto account = database::Account{};
      ia >> account;
      std::cout << confu_soci::structAsString (account) << std::endl;
    }
}

void
WebserviceController::setIsLoggedIn (std::string const &msg)
{
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () == 2)
    {
      boost::algorithm::split (splitMesssage, splitMesssage.at (1), boost::is_any_of (","));
      if (splitMesssage.size () == 2)
        {
          session.isLoggedIn = (splitMesssage.at (0) == "true") ? true : false;
          session.loggInMessageFromServer = splitMesssage.at (1);
        }
    }
}

void
WebserviceController::channelJoined (std::string const &msg)
{
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, msg, boost::is_any_of ("|"));
  if (splitMesssage.size () == 2)
    {
      auto channel = confu_boost::toObject<shared_class::JoinChannel> (splitMesssage.at (1)).channel;
      session.channelMessages.insert_or_assign (channel, std::vector<std::string>{});
    }
}

void
WebserviceController::broadcastedMessageForChannel (std::string const &msg)
{
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

std::vector<std::string>
WebserviceController::handleMessage (std::string const &msg)
{
  auto result = std::vector<std::string>{};
  if (boost::algorithm::starts_with (msg, "account|"))
    {
      createAccount (msg);
    }
  else if (boost::algorithm::starts_with (msg, "login result|"))
    {
      setIsLoggedIn (msg);
    }
  else if (boost::algorithm::starts_with (msg, "JoinChannel|"))
    {
      channelJoined (msg);
    }
  else if (boost::algorithm::starts_with (msg, "broadcasted message for channel|"))
    {
      broadcastedMessageForChannel (msg);
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
