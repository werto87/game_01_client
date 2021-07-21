#include "src/webservice/webservice.hxx"

EM_BOOL
onopen (int /*eventType*/, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData)
{
  std::cout << "on_open" << std::endl;
  auto &webservice = *static_cast<Webservice *> (userData);
  webservice.ws = websocketEvent->socket;
  return 42;
}
EM_BOOL
onerror (int /*eventType*/, const EmscriptenWebSocketErrorEvent * /*websocketEvent*/, void * /*userData*/)
{
  std::cout << "on_error" << std::endl;
  return 42;
}
EM_BOOL
onclose (int /*eventType*/, const EmscriptenWebSocketCloseEvent * /*websocketEvent */, void *userData)
{
  std::cout << "on_close" << std::endl;
  auto &webservice = *static_cast<Webservice *> (userData);
  webservice.ws = {};
  return 42;
}
EM_BOOL
onmessage (int /*eventType*/, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData)
{
  if (websocketEvent->isText)
    {
      std::stringstream ss{};
      ss << websocketEvent->data;
      static_cast<Webservice *> (userData)->handleMessage (ss.str ());
    }
  else
    {
      std::cerr << "binary currently not supported" << std::endl;
    }
  return EM_TRUE;
}