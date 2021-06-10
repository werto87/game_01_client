#ifndef EBE26351_4857_4382_9622_DE0400244EE9
#define EBE26351_4857_4382_9622_DE0400244EE9

#include <confu_json/to_json.hxx>
#include <game_01_shared_class/serialization.hxx>
#include <iostream>
template <typename T>
std::string
// only use this if your type is a simple user defined type with not template class something like namespace::MyType and you want to get MyType
// does not work with templated types or something like std::string which is std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > and the result will be allocator<char> >
typeNameWithOutNamespace (T const &)
{
  auto fullName = std::vector<std::string>{};
  auto typeWithNamespace = boost::typeindex::type_id<T> ().pretty_name ();
  boost::algorithm::split (fullName, typeWithNamespace, boost::is_any_of ("::"));
  return fullName.back ();
}

template <typename T>
std::string
typeName (T const &)
{
  return boost::typeindex::type_id<T> ().pretty_name ();
}

template <typename TypeToSend>
void
sendObject (std::deque<std::string> &msgToSend, TypeToSend const &typeToSend)
{
  std::stringstream ss{};
  ss << typeNameWithOutNamespace (typeToSend) << '|' << confu_json::to_json (typeToSend);
  std::cout << ss.str () << std::endl;
  msgToSend.push_back (ss.str ());
}

#endif /* EBE26351_4857_4382_9622_DE0400244EE9 */
