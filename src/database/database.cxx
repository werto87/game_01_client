#include "src/database/database.hxx"
#include <boost/lexical_cast.hpp>
#include <boost/optional/optional.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <filesystem>
#include <string>
#include <sys/types.h>
#include <system_error>

std::string pathToDatabase;
u_int32_t filePostfix;

namespace database
{

void
createEmptyDatabase ()
{
  filePostfix = 0;
#ifdef DEBUG
  // in debug mode its allowed to start multiple instances
  // NOTE every instance is started with a fresh database
  while (std::filesystem::exists (std::filesystem::path{ pathToDatabaseFolder }.append (databaseName + std::to_string (filePostfix))))
    {
      if (filePostfix == std::numeric_limits<u_int32_t>::max ())
        {
          std::cout << "its only allowed to run " + std::to_string (std::numeric_limits<u_int32_t>::max ()) + " clients" << std::endl;
          std::cout << "please check folder: " << pathToDatabaseFolder << std::endl;
          throw std::system_error{};
        }
      filePostfix++;
    }
#endif
  std::filesystem::create_directory (pathToDatabaseFolder);
  pathToDatabase = std::filesystem::path{ pathToDatabaseFolder }.append (databaseName + std::to_string (filePostfix));
  std::cout << pathToDatabase << std::endl;
  std::filesystem::copy_file (pathToTemplateDatabase, pathToDatabase, std::filesystem::copy_options::overwrite_existing);
}

void
createTables ()
{
  soci::session sql (soci::sqlite3, pathToDatabase);
  confu_soci::createTableForStruct<Character> (sql, { { "accountId", "Account", "id" } });
  confu_soci::createTableForStruct<Account> (sql);
  confu_soci::createTableForStruct<BoardElement> (sql, { { "boardId", "Board", "id" } });
  confu_soci::createTableForStruct<Board> (sql, { { "gameId", "Game", "id" } });
  confu_soci::createTableForStruct<Game> (sql);
}

boost::optional<Account>
createAccount (std::string const &firstName, std::string const &lastName)
{
  soci::session sql (soci::sqlite3, pathToDatabase);
  return confu_soci::findStruct<Account> (sql, "id", confu_soci::insertStruct (sql, Account{ .id = {}, .accountName = firstName, .password = lastName }, true, true));
}

boost::optional<database::Character>
createCharacter (std::string const &accoundId)
{
  soci::session sql (soci::sqlite3, pathToDatabase);
  return confu_soci::findStruct<Character> (sql, "id", confu_soci::insertStruct (sql, Character{ .id = {}, .positionX = {}, .positionY = {}, .positionZ = {}, .accountId = accoundId }, true, true));
}

} // namespace database