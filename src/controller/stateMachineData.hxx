#ifndef B6D64EAB_CF55_4504_9D9D_098D831157E9
#define B6D64EAB_CF55_4504_9D9D_098D831157E9

#include <deque>
#include <map>
#include <optional>
#include <string>
#include <vector>

struct ChatData
{
  std::string selectChannelComboBoxName ();

  std::vector<std::string> const &messagesForChannel (std::string const &channel);

  std::vector<std::string> channelNames ();

  std::optional<std::string> selectedChannelName;
  std::string channelToJoin;
  std::string messageToSendToChannel;
  std::map<std::string, std::vector<std::string>> channelMessages{};
  bool joinChannelClicked = false;
  bool sendMessageClicked = false;
};

struct MakeGameMachineData
{
  ChatData chatData{};
  std::string accountName{};
};

struct MessagesToSendToServer
{
  std::deque<std::string> messagesToSendToServer{};
};

#endif /* B6D64EAB_CF55_4504_9D9D_098D831157E9 */
