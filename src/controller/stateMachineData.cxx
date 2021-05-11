#include "src/controller/stateMachineData.hxx"
#include <pipes/pipes.hpp>

std::string
ChatData::selectChannelComboBoxName ()
{
  return selectedChannelName.value_or ("Select Channel");
}
std::vector<std::string> const &
ChatData::messagesForChannel (std::string const &channel)
{
  return channelMessages.at (channel);
}

std::vector<std::string>
ChatData::channelNames ()
{
  auto result = std::vector<std::string>{};
  channelMessages >>= pipes::transform ([] (auto const &channelAndMessages) { return std::get<0> (channelAndMessages); }) >>= pipes::push_back (result);
  return result;
}