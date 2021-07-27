#include "src/ui/screen.hxx"
#include "src/util/imgui_util/imgui_stdlib.h"
#include "src/util/util.hxx"
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>
#include <boost/numeric/conversion/cast.hpp>
#include <chrono>
#include <cstddef>
#include <durak/card.hxx>
#include <durak/game.hxx>
#include <durak/gameData.hxx>
#include <durak/print.hxx>
#include <fmt/core.h>
#include <game_01_shared_class/serialization.hxx>
#include <imgui.h>
#include <imgui_internal.h>
#include <iterator>
#include <magic_enum.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/all.hpp>
#include <sstream>
#include <string>
#include <type_traits>

namespace ImGui
{

bool
InputIntLimit (std::string const &label, int &value, std::array<int, 2> const &limit)
{
  ImGui::InputInt (label.c_str (), &value);
  if (value < limit.front ())
    {
      value = limit.front ();
    }
  else if (value > limit.back ())
    {
      value = limit.back ();
    }
  return ImGui::IsItemDeactivatedAfterEdit ();
}

void
PushDisabled (float alphaScale = 1.0f, bool disabled = true)
{
  if (disabled)
    {
      PushItemFlag (1 << 2 /*ImGuiItemFlags_Disabled*/, true);
      PushStyleVar (ImGuiStyleVar_Alpha, ImGui::GetStyle ().Alpha * alphaScale);
    }
}
void
PushDisabled (bool disabled, std::chrono::milliseconds const &time)
{
  if (disabled)
    {
      using namespace std::chrono_literals;
      if (time > 5s)
        {
          PushDisabled (0.8f);
        }
      else if (time > 200ms)
        {
          PushDisabled (0.9f);
        }
      else
        {
          PushDisabled ();
        }
    }
}

void
PopDisabled (bool disabled)
{
  if (disabled)
    {
      PopItemFlag ();
      PopStyleVar ();
    }
}

bool
DisableAndDimButtonOnTimeAndCondition (std::string const &label, bool disabled, ImVec2 const &size = ImVec2 (-1, 0), std::chrono::milliseconds const &timer = {})
{
  PushDisabled (disabled, timer);
  auto result = false;
  ImGui::Button (label.c_str (), size);
  PopDisabled (disabled);
  return result;
}

bool
DisableAndDimButtonOnCondition (std::string const &label, bool disabled, float alpha, ImVec2 const &size = ImVec2 (-1, 0))
{

  PushDisabled (alpha, disabled);
  auto result = false;
  result = ImGui::Button (label.c_str (), size);
  PopDisabled (disabled);
  return result;
}

template <IsEnum E>
bool
EnumCombo (E &myEnum)
{
  auto const oldValue = myEnum;
  auto item = std::string{ magic_enum::enum_name (myEnum) };
  auto current_item = item.c_str ();
  if (ImGui::BeginCombo ("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
    {
      for (auto n = 0UL; n < magic_enum::enum_count<E> (); n++)
        {
          auto tmpItem = std::string{ magic_enum::enum_name (static_cast<E> (n)) };
          auto tmp_current_item = tmpItem.c_str ();
          bool is_selected = (current_item == tmp_current_item); // You can store your selection however you want, outside or inside your objects
          if (ImGui::Selectable (tmp_current_item, is_selected)) myEnum = static_cast<E> (n);
          if (is_selected) ImGui::SetItemDefaultFocus (); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }
      ImGui::EndCombo ();
    }
  return oldValue != myEnum;
}

} // namespace ImGui

void
drawType (durak::Type type)
{
  switch (type)
    {
    case durak::Type::clubs:
      {
        ImGui::TextUnformatted (from_u8string (std::u8string{ u8"♣" }).c_str ());
        break;
      }
    case durak::Type::spades:
      {
        ImGui::TextUnformatted (from_u8string (std::u8string{ u8"♠" }).c_str ());
        break;
      }
    case durak::Type::hearts:
      {
        ImGui::PushStyleColor (ImGuiCol_Text, ImVec4 (ImColor (255, 0, 0)));
        ImGui::TextUnformatted (from_u8string (std::u8string{ u8"♥" }).c_str ());
        ImGui::PopStyleColor ();
        break;
      }
    case durak::Type::diamonds:
      {
        ImGui::PushStyleColor (ImGuiCol_Text, ImVec4 (ImColor (255, 0, 0)));
        ImGui::TextUnformatted (from_u8string (std::u8string{ u8"♦" }).c_str ());
        ImGui::PopStyleColor ();
        break;
      }
    }
}

void
drawCard (durak::Card const &card)
{
  if (card.type == durak::Type::clubs || card.type == durak::Type::spades)
    {
      ImGui::TextUnformatted (fmt::format ("{}, {}", card.value, (card.type) == durak::Type::spades ? from_u8string (std::u8string{ u8"♠" }).c_str () : from_u8string (std::u8string{ u8"♣" }).c_str ()).c_str ());
    }
  else
    {
      ImGui::PushStyleColor (ImGuiCol_Text, ImVec4 (ImColor (255, 0, 0)));
      ImGui::TextUnformatted (fmt::format ("{}, {}", card.value, (card.type) == durak::Type::hearts ? from_u8string (std::u8string{ u8"♥" }).c_str () : from_u8string (std::u8string{ u8"♦" }).c_str ()).c_str ());
      ImGui::PopStyleColor ();
    }
}
// namespace ImGui
// {
// void
// LoadingIndicatorCircle (const char *label, const float indicator_radius, const ImVec4 &main_color, const ImVec4 &backdrop_color, const int circle_count, const float speed)
// {
//   ImGuiWindow *window = GetCurrentWindow ();
//   if (window->SkipItems)
//     {
//       return;
//     }

//   ImGuiContext &g = *GImGui;
//   const ImGuiID id = window->GetID (label);

//   ImVec2 pos = window->DC.CursorPos;

//   const float circle_radius = indicator_radius / 8.0f;
//   // pos.y += (indicator_radius / 2) - (circle_radius * 2);
//   // pos.x -= 270;
//   const ImRect bb (pos, ImVec2 (pos.x + indicator_radius * 2.0f, pos.y + indicator_radius * 2.0f));
//   ItemSize (bb, 1);
//   if (!ItemAdd (bb, id))
//     {
//       return;
//     }
//   const float t = g.Time;
//   const auto degree_offset = 2.0f * IM_PI / circle_count;
//   for (int i = 0; i < circle_count; ++i)
//     {
//       const auto x = indicator_radius * std::sin (degree_offset * i);
//       const auto y = indicator_radius * std::cos (degree_offset * i);
//       const auto growth = std::max (0.0f, std::sin (t * speed - i * degree_offset));
//       ImVec4 color;
//       color.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
//       color.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
//       color.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
//       color.w = growth;
//       window->DrawList->AddCircleFilled (ImVec2 (pos.x + indicator_radius + x, pos.y + indicator_radius - y), circle_radius + growth * circle_radius, GetColorU32 (color));
//     }
// }
// }

void
chatScreen (ChatData &chatData, bool shouldLockScreen, std::chrono::milliseconds const &time)
{
  ImGui::PushItemWidth (-1);
  ImGui::PushDisabled (shouldLockScreen, time);
  ImGui::TextUnformatted ("Join Channel");
  ImGui::InputText ("##JoinChannel", &chatData.channelToJoin);
  chatData.joinChannelClicked = ImGui::Button ("Join Channel", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen);
  auto channelNames = chatData.channelNames ();
  if (ImGui::BeginCombo ("##combo 1", chatData.selectChannelComboBoxName ().c_str ()))
    {
      if ((not chatData.selectedChannelName && not channelNames.empty ()) || (chatData.selectedChannelName && not channelNames.empty () && ranges::find (channelNames, chatData.selectedChannelName) == channelNames.end ()))
        {
          chatData.selectedChannelName = channelNames.front ();
        }
      for (auto &&channelName : channelNames)
        {
          const bool is_selected = (chatData.selectedChannelName == channelName);
          if (ImGui::Selectable (channelName.data (), is_selected)) chatData.selectedChannelName = channelName;
          if (is_selected) ImGui::SetItemDefaultFocus ();
        }
      ImGui::EndCombo ();
    }
  ImGui::BeginChild ("scrolling", ImVec2 (0, 500), false, ImGuiWindowFlags_HorizontalScrollbar);
  if (chatData.selectedChannelName && ranges::find (channelNames, chatData.selectedChannelName) != channelNames.end ())
    {
      for (auto const &text : chatData.channelMessages.at (chatData.selectedChannelName.value ()))
        {
          ImGui::TextUnformatted (text.data (), text.data () + text.size ());
        }
      if (ImGui::GetScrollY () >= ImGui::GetScrollMaxY ()) ImGui::SetScrollHereY (1.0f);
    }
  ImGui::EndChild ();
  ImGui::PushDisabled (shouldLockScreen, time);
  ImGui::TextUnformatted ("Send to Channel");
  ImGui::InputText ("##SendToChannel", &chatData.messageToSendToChannel);
  chatData.sendMessageClicked = ImGui::Button ("Send to Channel", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen);
  ImGui::PopItemWidth ();
}

void
createGameLobbyScreen (CreateGameLobby &createGameLobby, std::optional<WaitForServer> &waitForServer, std::string accountName, ChatData &chatData)
{
  ImGui::PushItemWidth (-1);
  auto const shouldLockScreen = waitForServer.has_value ();
  auto time = std::chrono::milliseconds{};
  if (waitForServer)
    {
      time = std::chrono::duration_cast<std::chrono::milliseconds> (waitForServer->elapsedTime ());
    }
  chatScreen (chatData, shouldLockScreen, time);
  if (not createGameLobby.accountNamesInGameLobby.empty () && accountName == createGameLobby.accountNamesInGameLobby.at (0))
    {
      ImGui::TextUnformatted ("set max user count: ");
      ImGui::SameLine ();
      ImGui::PushDisabled (shouldLockScreen, time);
      createGameLobby.sendMaxUserCountClicked = ImGui::InputIntLimit ("##MaxUserCount", createGameLobby.maxUserInGameLobby, { boost::numeric_cast<int> (createGameLobby.accountNamesInGameLobby.size ()), std::numeric_limits<int>::max () });
      ImGui::PopDisabled (shouldLockScreen);
      ImGui::TextUnformatted ("set max card value: ");
      ImGui::SameLine ();
      ImGui::PushDisabled (shouldLockScreen, time);
      createGameLobby.sendMaxCardValueClicked = ImGui::InputIntLimit ("##maxCardValue", createGameLobby.maxCardValue, { 1, std::numeric_limits<int>::max () });
      ImGui::PopDisabled (shouldLockScreen);
      ImGui::TextUnformatted ("Timer Type: ");
      ImGui::SameLine ();
      ImGui::PushDisabled (shouldLockScreen, time);
      createGameLobby.sendTimerOptionClicked = ImGui::EnumCombo (createGameLobby.timerOption.timerType);
      ImGui::PopDisabled (shouldLockScreen);
      if (createGameLobby.timerOption.timerType != shared_class::TimerType::noTimer)
        {
          ImGui::TextUnformatted ("Time at Start in Seconds: ");
          ImGui::SameLine ();
          ImGui::PushDisabled (shouldLockScreen, time);
          createGameLobby.sendTimerOptionClicked |= ImGui::InputIntLimit ("##timeAtStartInSeconds", createGameLobby.timerOption.timeAtStartInSeconds, { 0, std::numeric_limits<int>::max () });
          ImGui::PopDisabled (shouldLockScreen);
          ImGui::TextUnformatted ("Time for each Round in Seconds: ");
          ImGui::SameLine ();
          ImGui::PushDisabled (shouldLockScreen, time);
          createGameLobby.sendTimerOptionClicked |= ImGui::InputIntLimit ("##timeForEachRoundInSeconds", createGameLobby.timerOption.timeForEachRoundInSeconds, { 0, std::numeric_limits<int>::max () });
          ImGui::PopDisabled (shouldLockScreen);
        }
    }
  else
    {
      ImGui::TextUnformatted (std::string{ "max user count: " + std::to_string (createGameLobby.maxUserInGameLobby) }.c_str ());
      ImGui::TextUnformatted (std::string{ "max card value: " + std::to_string (createGameLobby.maxCardValue) }.c_str ());
      ImGui::TextUnformatted (std::string{ "Timer Type: " + std::string{ magic_enum::enum_name (createGameLobby.timerOption.timerType) } }.c_str ());
      if (createGameLobby.timerOption.timerType != shared_class::TimerType::noTimer)
        {
          ImGui::TextUnformatted (fmt::format ("Time at Start in Seconds:  {}", createGameLobby.timerOption.timeAtStartInSeconds).c_str ());
          ImGui::TextUnformatted (fmt::format ("Time for each Round in Seconds: {}", createGameLobby.timerOption.timeForEachRoundInSeconds).c_str ());
        }
    }
  ImGui::TextUnformatted ("user in lobby:");
  for (auto &lobbyMemberAccountName : createGameLobby.accountNamesInGameLobby)
    {
      ImGui::TextUnformatted (lobbyMemberAccountName.c_str ());
    }
  ImGui::PushDisabled (shouldLockScreen, time);
  if (not createGameLobby.accountNamesInGameLobby.empty () && accountName == createGameLobby.accountNamesInGameLobby.at (0))
    {
      createGameLobby.startGame = ImGui::Button ("Start Game", ImVec2 (-1, 0));
    }
  createGameLobby.leaveGameLobby = ImGui::Button ("Leave Game Lobby", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen);
  ImGui::PopItemWidth ();
}

void
messageBoxPopupScreen (MessageBoxPopup &messageBoxPopup, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont)
{
  ImGui::PushItemWidth (-1);
  auto time = std::chrono::milliseconds{};
  if (waitForServer)
    {
      time = std::chrono::duration_cast<std::chrono::milliseconds> (waitForServer->elapsedTime ());
    }
  ImGui::Dummy (ImVec2 (0.0f, (windowHeight - ((ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
  ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
  ImGui::Dummy (ImVec2 ((windowWidth - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
  ImGui::PushFont (&biggerFont);
  ImGui::PopFont ();
  ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
  ImGui::SameLine ();
  ImGui::BeginChild ("ChildR", ImVec2 (windowWidth / 2, (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 40), true, window_flags);
  ImGui::Dummy (ImVec2 (0.0f, 10.0f));
  ImGui::Dummy (ImVec2 (10.0f, 0.0f));
  ImGui::SameLine ();
  ImGui::PushStyleVar (ImGuiStyleVar_ChildBorderSize, 0);
  ImGui::BeginChild ("ChildR_sub", ImVec2 ((windowWidth / 2) - 50, (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 10), true, window_flags);
  ImGui::PopStyleVar ();
  ImGui::TextUnformatted (messageBoxPopup.message.c_str ());
  ImGui::EndChild ();
  ImGui::EndChild ();
  ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
  for (auto &button : messageBoxPopup.buttons)
    {
      ImGui::SameLine ();
      ImGui::PushDisabled (button.disabled, time);
      button.pressed = ImGui::Button (button.name.c_str ());
      ImGui::PopDisabled (button.disabled);
    }
  ImGui::PopStyleVar ();
  ImGui::PopItemWidth ();
}

void
loginScreen (Login &data, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont)
{
  ImGui::PushItemWidth (-1);
  auto const shouldLockScreen = waitForServer.has_value ();
  auto time = std::chrono::milliseconds{};
  if (waitForServer)
    {
      time = std::chrono::duration_cast<std::chrono::milliseconds> (waitForServer->elapsedTime ());
    }
  ImGui::PushDisabled (shouldLockScreen, time);
  ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
  ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Sign in to Modern Durak").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
  ImGui::SameLine ();
  ImGui::PushFont (&biggerFont);
  ImGui::TextUnformatted ("Sign in to Modern Durak");
  ImGui::PopFont ();
  ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
  ImGui::SameLine ();
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;
  ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
  ImGui::BeginChild ("ChildR", ImVec2 (windowWidth / 2, (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 50), true, window_flags);
  ImGui::PopStyleVar ();
  ImGui::Dummy (ImVec2 (0.0f, 10.0f));
  ImGui::Dummy (ImVec2 (10.0f, 0.0f));
  ImGui::SameLine ();
  ImGui::BeginChild ("ChildR_sub", ImVec2 ((windowWidth / 2) - 50, (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 30), false, window_flags);
  ImGui::PushItemWidth (-1.0f);
  ImGui::TextUnformatted ("Username");
  ImGui::InputText ("##username", &data.accountName);
  ImGui::TextUnformatted ("Password");
  ImGui::InputText ("##password", &data.password, ImGuiInputTextFlags_Password);
  ImGui::PopDisabled (shouldLockScreen);
  if (shouldLockScreen)
    {
      auto &button = waitForServer->buttons.front ();
      using namespace std::chrono_literals;
      if (waitForServer->elapsedTime () >= 5s)
        {
          button.pressed = ImGui::Button (button.name.c_str (), ImVec2 (-1, 0));
        }
      else
        {
          ImGui::PushDisabled (shouldLockScreen, time);
          ImGui::Button ("Sign in", ImVec2 (-1, 0));
          ImGui::PopDisabled (shouldLockScreen);
        }
    }
  else
    {
      data.loginClicked = ImGui::Button ("Sign in", ImVec2 (-1, 0));
    }
  ImGui::PushDisabled (shouldLockScreen, time);
  ImGui::PopItemWidth ();
  ImGui::EndChild ();
  ImGui::EndChild ();
  ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
  ImGui::SameLine ();
  ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
  ImGui::BeginChild ("ChildR123", ImVec2 (windowWidth / 2, (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 20), true, window_flags);
  ImGui::Dummy (ImVec2 (((windowWidth / 2) - (ImGui::CalcTextSize ("New to MD?").x + ImGui::CalcTextSize ("create an account").x + (ImGui::GetStyle ().ItemSpacing.x * 6))) / 2, 0.0f));
  ImGui::SameLine ();
  ImGui::TextUnformatted ("New to MD?");
  ImGui::SameLine ();
  data.createAccountClicked = ImGui::SmallButton ("Create an Account");
  ImGui::PopDisabled (shouldLockScreen);
  ImGui::PopStyleVar ();
  ImGui::EndChild ();
  ImGui::PopItemWidth ();
}

void
createAccountScreen (CreateAccount &data, std::optional<WaitForServer> &waitForServer, float windowWidth, float windowHeight, ImFont &biggerFont)
{
  ImGui::PushItemWidth (-1);
  auto const shouldLockScreen = waitForServer.has_value ();
  auto time = std::chrono::milliseconds{};
  if (waitForServer)
    {
      time = std::chrono::duration_cast<std::chrono::milliseconds> (waitForServer->elapsedTime ());
    }
  ImGui::PushDisabled (shouldLockScreen, time);
  ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;
  ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
  ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Create your Account").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
  ImGui::SameLine ();
  ImGui::PushFont (&biggerFont);
  ImGui::TextUnformatted ("Create your Account");
  ImGui::PopFont ();
  ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
  ImGui::SameLine ();
  ImGui::BeginChild ("ChildR", ImVec2 (windowWidth / 2, (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 50), true, window_flags);
  ImGui::Dummy (ImVec2 (0.0f, 10.0f));
  ImGui::Dummy (ImVec2 (10.0f, 0.0f));
  ImGui::SameLine ();
  ImGui::PushStyleVar (ImGuiStyleVar_ChildBorderSize, 0);
  ImGui::BeginChild ("ChildR_sub", ImVec2 ((windowWidth / 2) - 50, (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 10), true, window_flags);
  ImGui::PopStyleVar ();
  ImGui::PushItemWidth (-1.0f);
  ImGui::TextUnformatted ("Username");
  ImGui::InputText ("##account-username", &data.accountName);
  ImGui::TextUnformatted ("Password");
  ImGui::InputText ("##account-password", &data.password, ImGuiInputTextFlags_Password);
  data.backToLoginClicked = ImGui::Button ("Back");
  ImGui::PopDisabled (shouldLockScreen);
  ImGui::SameLine ();
  using namespace std::chrono_literals;
  if (shouldLockScreen && time > 5s)
    {
      auto &button = waitForServer->buttons.front ();
      button.pressed = ImGui::Button (button.name.c_str (), ImVec2 (-1, 0));
    }
  else
    {
      ImGui::PushDisabled (shouldLockScreen, time);
      data.createAccountClicked = ImGui::Button ("Create Account and Sign in", ImVec2 (-1, 0));
      ImGui::PopDisabled (shouldLockScreen);
    }
  ImGui::PopItemWidth ();
  ImGui::EndChild ();
  ImGui::EndChild ();
  ImGui::PopStyleVar ();
}

void
lobbyScreen (Lobby &data, std::optional<WaitForServer> &waitForServer, ChatData &chatData)
{
  ImGui::PushItemWidth (-1);
  auto const shouldLockScreen = waitForServer.has_value ();
  auto time = std::chrono::milliseconds{};
  if (waitForServer)
    {
      time = std::chrono::duration_cast<std::chrono::milliseconds> (waitForServer->elapsedTime ());
    }
  chatScreen (chatData, shouldLockScreen, time);
  ImGui::PushDisabled (shouldLockScreen, time);
  ImGui::TextUnformatted ("Create Game Lobby");
  ImGui::TextUnformatted ("Game Lobby Name");
  ImGui::InputText ("##CreateGameLobbyName", &data.createGameLobbyName);
  ImGui::TextUnformatted ("Game Lobby Password");
  ImGui::InputText ("##CreateGameLobbyPassword", &data.createGameLobbyPassword);
  data.createCreateGameLobbyClicked = ImGui::Button ("Create Game Lobby", ImVec2 (-1, 0));
  ImGui::TextUnformatted ("Join Game Lobby");
  ImGui::TextUnformatted ("Game Lobby Name");
  ImGui::InputText ("##JoinGameLobbyName", &data.joinGameLobbyName);
  ImGui::TextUnformatted ("Game Lobby Password");
  ImGui::InputText ("##JoinGameLobbyPassword", &data.joinGameLobbyPassword);
  data.createJoinGameLobbyClicked = ImGui::Button ("Join Game Lobby", ImVec2 (-1, 0));
  data.logoutButtonClicked = ImGui::Button ("Logout", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen);
  ImGui::PopItemWidth ();
}

void
timeLeft (shared_class::DurakTimers const &timers, std::string const &playerNameToLookFor)
{
  using namespace std::chrono;
  if (auto pausedTimer = ranges::find_if (timers.pausedTimeUserDurationMilliseconds,
                                          [playerNameToLookFor] (auto const &playerNameAndTimer) {
                                            auto [playerName, timer] = playerNameAndTimer;
                                            return playerName == playerNameToLookFor;
                                          });
      pausedTimer != timers.pausedTimeUserDurationMilliseconds.end ())
    {
      ImGui::TextUnformatted (fmt::format ("Time left: {}", duration_cast<seconds> (milliseconds (pausedTimer->second)).count ()).c_str ());
    }
  else if (auto runningTimer = ranges::find_if (timers.runningTimeUserTimePointMilliseconds,
                                                [playerNameToLookFor] (auto const &playerNameAndTimer) {
                                                  auto [playerName, timer] = playerNameAndTimer;
                                                  return playerName == playerNameToLookFor;
                                                });
           runningTimer != timers.runningTimeUserTimePointMilliseconds.end ())
    {
      auto const timeOutTimePoint = time_point<system_clock, milliseconds>{ milliseconds{ runningTimer->second } };
      ImGui::TextUnformatted (fmt::format ("Time left: {}", duration_cast<seconds> (timeOutTimePoint - system_clock::now ()).count ()).c_str ());
    }
}

bool
notAllowedMove (std::vector<durak::Move> allowedMoves, durak::Move moveToCheck)
{
  return ranges::find (allowedMoves, moveToCheck) == allowedMoves.end ();
}

void
gameScreen (Game &game, std::optional<WaitForServer> &waitForServer, std::string const &accountName, ChatData &chatData)
{
  ImGui::TextUnformatted (std::string{ "Round: " + std::to_string (game.gameData.round) }.c_str ());
  ImGui::PushItemWidth (-1);
  auto const shouldLockScreen = waitForServer.has_value ();
  auto time = std::chrono::milliseconds{};
  if (waitForServer)
    {
      time = std::chrono::duration_cast<std::chrono::milliseconds> (waitForServer->elapsedTime ());
    }
  chatScreen (chatData, shouldLockScreen, time);
  ImGui::TextUnformatted ("Table");
  if (game.gameData.table.empty ())
    {
      ImGui::SameLine ();
      ImGui::TextUnformatted ("Empty Table");
    }
  if (not game.selectedCardFromTable || game.gameData.table.size () <= game.selectedCardFromTable.value () || game.gameData.table.at (game.selectedCardFromTable.value ()).second)
    {
      // old selection is not valid because of optional card has value so we try to set the selection to first free element
      if (auto cardAndOptionalCardItr = ranges::find_if (game.gameData.table, [] (auto const &cardAndOptionalCard) { return not cardAndOptionalCard.second.has_value (); }); cardAndOptionalCardItr != game.gameData.table.end ())
        {
          game.selectedCardFromTable = std::distance (game.gameData.table.begin (), cardAndOptionalCardItr);
        }
      else
        {
          game.selectedCardFromTable = {};
        }
    }
  auto currentPlayer = ranges::find_if (game.gameData.players, [&accountName] (auto const &_player) { return accountName == _player.name; });
  auto currentPlayerRole = durak::PlayerRole::waiting;
  if (currentPlayer != game.gameData.players.end ())
    {
      currentPlayerRole = currentPlayer->playerRole;
    }
  for (int i = 0; game.gameData.table.size () > boost::numeric_cast<size_t> (i); i++)
    {
      auto const &[card, optionalCard] = game.gameData.table.at (boost::numeric_cast<size_t> (i));
      drawCard (card);
      if (optionalCard)
        {
          ImGui::SameLine ();
          drawCard (optionalCard.value ());
        }
      if (game.selectedCardFromTable && currentPlayerRole == durak::PlayerRole::defend && not optionalCard)
        {
          int selectedValue = boost::numeric_cast<int> (game.selectedCardFromTable.value ());
          ImGui::SameLine ();
          ImGui::RadioButton (std::string{ "##defend" + std::to_string (i) }.c_str (), &selectedValue, i);
          game.selectedCardFromTable = boost::numeric_cast<size_t> (selectedValue);
        }
    }
  ImGui::TextUnformatted (fmt::format ("Cards in Deck: {}", game.gameData.cardsInDeck).c_str ());
  if (game.gameData.lastCardInDeck)
    {
      ImGui::TextUnformatted ("Last Card:");
      ImGui::SameLine ();
      drawCard (game.gameData.lastCardInDeck.value ());
    }
  ImGui::TextUnformatted ("Trump:");
  ImGui::SameLine ();
  drawType (game.gameData.trump);
  if (currentPlayer != game.gameData.players.end ())
    {
      for (size_t i = 0; i < currentPlayer->cards.size (); i++)
        {
          if (currentPlayer->playerRole == durak::PlayerRole::defend)
            {
              int selectedValue = 0;
              if (auto selectedCardsItr = ranges::find_if (game.selectedCards, [] (bool selected) { return selected; }); selectedCardsItr != game.selectedCards.end ())
                {
                  selectedValue = static_cast<int> (std::distance (game.selectedCards.begin (), selectedCardsItr));
                }
              ImGui::RadioButton (std::string{ "##" + std::to_string (i) }.c_str (), &selectedValue, static_cast<int> (i));
              game.selectedCards = std::vector<bool> (game.selectedCards.size ());
              game.selectedCards.at (static_cast<size_t> (selectedValue)) = true;
            }
          else if (currentPlayer->playerRole == durak::PlayerRole::attack || currentPlayer->playerRole == durak::PlayerRole::assistAttacker)
            {
              bool tempBool = game.selectedCards.at (i);
              ImGui::Checkbox (std::string{ "##" + std::to_string (i) }.c_str (), &tempBool);
              game.selectedCards.at (i) = tempBool;
            }
          auto const &cardOptional = currentPlayer->cards.at (i);
          if (cardOptional)
            {
              ImGui::SameLine ();
              drawCard (cardOptional.value ());
            }
          else
            {
              ImGui::SameLine ();
              ImGui::TextUnformatted ("Card: {? ,?}");
            }
        }
    }
  timeLeft (game.timers, currentPlayer->name);
  ImGui::TextUnformatted ("Other Players:");
  for (auto const &player : game.gameData.players | ranges::views::filter ([&accountName] (durak::PlayerData const &playerData) { return accountName != playerData.name; }))
    {
      ImGui::TextUnformatted (fmt::format ("Name: {} Role: {} Cards: {}", player.name, magic_enum::enum_name (player.playerRole), std::to_string (player.cards.size ())).c_str ());
      timeLeft (game.timers, player.name);
    }
  if (currentPlayerRole == durak::PlayerRole::attack || currentPlayerRole == durak::PlayerRole::assistAttacker)
    {
      auto const disabled = notAllowedMove (game.allowedMoves.allowedMoves, durak::Move::addCard) && notAllowedMove (game.allowedMoves.allowedMoves, durak::Move::startAttack);
      game.placeSelectedCardsOnTable = ImGui::DisableAndDimButtonOnCondition ("Place selected Cards on Table", disabled, 0.8f);
    }
  else if (currentPlayerRole == durak::PlayerRole::defend)
    {
      game.placeSelectedCardsOnTable = ImGui::DisableAndDimButtonOnCondition ("Place selected Card on Table to defend", notAllowedMove (game.allowedMoves.allowedMoves, durak::Move::defend), 0.8f);
    }
  if (currentPlayerRole == durak::PlayerRole::defend)
    {
      game.pass = ImGui::DisableAndDimButtonOnCondition ("Draw Cards from Table", notAllowedMove (game.allowedMoves.allowedMoves, durak::Move::takeCards), 0.8f);
    }
  else if (currentPlayerRole == durak::PlayerRole::attack || currentPlayerRole == durak::PlayerRole::assistAttacker)
    {
      game.pass = ImGui::DisableAndDimButtonOnCondition ("Pass", notAllowedMove (game.allowedMoves.allowedMoves, durak::Move::pass), 0.8f);
    }
  game.surrender = ImGui::Button ("Surrender", ImVec2 (-1, 0));
  ImGui::PopItemWidth ();
}
