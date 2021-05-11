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
  std::string accountName;
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
  using timer = std::chrono::system_clock;
  timer::time_point clock_wait;
  timer::duration
  elapsedTime ()
  {
    return timer::now () - clock_wait;
  }
  std::optional<std::string> message{};
  std::vector<std::pair<std::string, bool>> buttons{ { "Cancel Sign in", false } };

private:
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
  using timer = std::chrono::system_clock;
  timer::time_point clock_wait;
  timer::duration
  elapsedTime ()
  {
    return timer::now () - clock_wait;
  }
  std::optional<std::string> message{};
  std::vector<std::pair<std::string, bool>> buttons{ { "Cancel", false }, { "Create Account", false } };
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
