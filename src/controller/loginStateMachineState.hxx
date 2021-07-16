#ifndef A268F183_8935_4464_9486_193D919FA140
#define A268F183_8935_4464_9486_193D919FA140

#include <chrono>
#include <optional>
#include <string>
#include <vector>

struct loginStateMachine
{
};
struct makeGameMachine
{
};
struct goToCreateGameLobby
{
};

struct goToGame
{
};

struct login
{
};
struct loginWaitForServer
{
};
struct createAccount
{
};
struct createAccountWaitForServer
{
};

struct Login
{
  std::string accountName{};
  std::string password{};
  bool createAccountClicked = false;
  bool loginClicked = false;
};
struct LoginWaitForServer
{
};
struct LoginError
{
  std::string message{};
  bool backToLoginClicked{};
};
struct CreateAccount
{
  std::string accountName{};
  std::string password{};
  bool createAccountClicked = false;
  bool backToLoginClicked{};
};
struct CreateAccountWaitForServer
{
};
struct CreateAccountError
{
  std::string message{};
  bool backToAccountClicked{};
};
struct CreateAccountSuccess
{
  std::string message{};
  bool backToAccountClicked{};
};

#endif /* A268F183_8935_4464_9486_193D919FA140 */
