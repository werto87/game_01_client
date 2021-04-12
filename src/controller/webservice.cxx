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
#include <iostream>
#include <sstream>

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

std::vector<std::string>
handleMessage (std::string const &msg)
{
  auto result = std::vector<std::string>{};
  if (boost::algorithm::starts_with (msg, "account|"))
    {
      createAccount (msg);
    }
  else if (boost::algorithm::starts_with (msg, "login result|"))
    {
    }
  return result;
}
