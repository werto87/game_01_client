#ifndef F06EFE63_F044_49C0_9D23_F73303313BC1
#define F06EFE63_F044_49C0_9D23_F73303313BC1

#include "src/model/session.hxx"
#include <deque>
#include <string>
#include <vector>
class WebserviceController
{
public:
  static std::vector<std::string> handleMessage (std::string const &msg);
  static void sendMessage (std::string const &msg);
  static void sendMessage (std::vector<std::string> &&msg);
  static bool messageEmpty ();
  static std::string popFront ();
  static bool
  hasLoginState ()
  {
    return session.isLoggedIn.has_value ();
  }
  static bool
  isLoggedIn ()
  {
    return session.isLoggedIn.value ();
  }
  static void
  resetSession ()
  {
    session = Session{};
  }

  static std::string
  logInMessageFromServer ()
  {
    return session.loggInMessageFromServer.value ();
  }

private:
  static void setIsLoggedIn (std::string const &msg);
  static std::deque<std::string> msgToSend;
  static Session session;
};
#endif /* F06EFE63_F044_49C0_9D23_F73303313BC1 */
