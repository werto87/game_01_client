#include "src/model/database.hxx"
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

static std::string pathToDatabase{};

void
createEmptyDatabase ()
{
  std::string const pathToTemplateDatabase = std::filesystem::path{ PATH_TO_BINARY }.parent_path ().append ("template_database").append ("empty_test_db");
  std::string const pathToDatabaseFolder = std::filesystem::path{ PATH_TO_BINARY }.append ("database");
  std::string const databaseName = "game_db";
  auto filePostfix = 0u;
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
  std::filesystem::copy_file (pathToTemplateDatabase, pathToDatabase, std::filesystem::copy_options::skip_existing);
}

void
createTables ()
{
  soci::session sql (soci::sqlite3, pathToDatabase);
  confu_soci::createTableForStruct<database::Character> (sql, { { "accountId", "Account", "id" } });
  confu_soci::createTableForStruct<database::Account> (sql);
  confu_soci::createTableForStruct<database::BoardElement> (sql, { { "boardId", "Board", "id" } });
  confu_soci::createTableForStruct<database::Board> (sql, { { "gameId", "Game", "id" } });
  confu_soci::createTableForStruct<database::Game> (sql);
}

void
upsertAccount (database::Account const &account)
{
  soci::session sql (soci::sqlite3, pathToDatabase);
  confu_soci::upsertStruct (sql, account, true);
}