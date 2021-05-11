#ifndef EBE26351_4857_4382_9622_DE0400244EE9
#define EBE26351_4857_4382_9622_DE0400244EE9

#include <confu_boost/confuBoost.hxx>
#include <confu_soci/convenienceFunctionForSoci.hxx>

template <typename TypeToSend>
void
sendObject (std::deque<std::string> &msgToSend, TypeToSend const &typeToSend)
{
  msgToSend.push_back (confu_soci::typeNameWithOutNamespace (typeToSend) + '|' + confu_boost::toString (typeToSend));
}

#endif /* EBE26351_4857_4382_9622_DE0400244EE9 */
