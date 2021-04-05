#ifndef AA8179A7_F57B_4110_97DC_CEBEDA7522AB
#define AA8179A7_F57B_4110_97DC_CEBEDA7522AB

#include <filesystem>
#include <string>
#include <sys/types.h>

std::string const pathToTemplateDatabase = std::filesystem::path{ PATH_TO_BINARY }.parent_path ().append ("template_database").append ("empty_test_db");
std::string const pathToDatabaseFolder = std::filesystem::path{ PATH_TO_BINARY }.append ("database");
std::string const databaseName = "game_db";

extern u_int32_t filePostfix;
extern std::string pathToDatabase;

#endif /* AA8179A7_F57B_4110_97DC_CEBEDA7522AB */
