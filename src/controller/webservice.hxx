#ifndef F06EFE63_F044_49C0_9D23_F73303313BC1
#define F06EFE63_F044_49C0_9D23_F73303313BC1

#include <string>
#include <vector>

std::vector<std::string> handleMessage (std::string const &msg);

void sendMessage (std::string const &msg);
void sendMessage (std::vector<std::string> &&msg);
bool messageEmpty ();
std::string popFront ();

#endif /* F06EFE63_F044_49C0_9D23_F73303313BC1 */
