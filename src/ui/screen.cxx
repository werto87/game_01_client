#include "src/ui/screen.hxx"
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>
#include <bits/ranges_algo.h>
#include <chrono>
#include <cstddef>
#include <durak/gameData.hxx>
#include <durak/print.hxx>
#include <fmt/core.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <ranges>
#include <sstream>
#include <string>
namespace ImGui
{
inline void
PushDisabled (bool enabled, std::chrono::milliseconds const &time)
{
  if (enabled)
    {
      extern void PushItemFlag (int option, bool enabled);
      PushItemFlag (1 << 2 /*ImGuiItemFlags_Disabled*/, !false);
      using namespace std::chrono_literals;

      if (time > 5s)
        {
          PushStyleVar (ImGuiStyleVar_Alpha, ImGui::GetStyle ().Alpha * (false ? 1.0f : 0.8f));
        }
      else if (time > 200ms)
        {
          PushStyleVar (ImGuiStyleVar_Alpha, ImGui::GetStyle ().Alpha * (false ? 1.0f : 0.9f));
        }
    }
}

inline void
PopDisabled (bool enabled, std::chrono::milliseconds const &time)
{
  if (enabled)
    {
      extern void PopItemFlag ();
      PopItemFlag ();
      using namespace std::chrono_literals;
      if (time > 200ms)
        {
          PopStyleVar ();
        }
    }
}

} // namespace ImGui

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
  ImGui::Text ("Join Channel");
  ImGui::InputText ("##JoinChannel", &chatData.channelToJoin);
  chatData.joinChannelClicked = ImGui::Button ("Join Channel", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen, time);
  auto channelNames = chatData.channelNames ();
  if (ImGui::BeginCombo ("##combo 1", chatData.selectChannelComboBoxName ().c_str ()))
    {
      if ((not chatData.selectedChannelName && not channelNames.empty ()) || (chatData.selectedChannelName && not channelNames.empty () && std::ranges::find (channelNames, chatData.selectedChannelName) == channelNames.end ()))
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
  if (chatData.selectedChannelName && std::ranges::find (channelNames, chatData.selectedChannelName) != channelNames.end ())
    {
      for (auto const &text : chatData.channelMessages.at (chatData.selectedChannelName.value ()))
        {
          ImGui::TextUnformatted (text.data (), text.data () + text.size ());
        }
      if (ImGui::GetScrollY () >= ImGui::GetScrollMaxY ()) ImGui::SetScrollHereY (1.0f);
    }
  ImGui::EndChild ();
  ImGui::PushDisabled (shouldLockScreen, time);
  ImGui::Text ("Send to Channel");
  ImGui::InputText ("##SendToChannel", &chatData.messageToSendToChannel);
  chatData.sendMessageClicked = ImGui::Button ("Send to Channel", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen, time);
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
      ImGui::Text ("set max user count: ");
      ImGui::PushDisabled (shouldLockScreen, time);
      ImGui::InputInt ("##MaxUserCount", &createGameLobby.maxUserInGameLobby);
      ImGui::PopDisabled (shouldLockScreen, time);
      ImGui::PushDisabled (shouldLockScreen, time);
      createGameLobby.sendMaxUserCountClicked = ImGui::Button ("set max user count", ImVec2 (-1, 0));
      ImGui::PopDisabled (shouldLockScreen, time);
    }
  else
    {
      ImGui::Text (std::string{ "max user count: " + std::to_string (createGameLobby.maxUserInGameLobby) }.c_str ());
    }
  ImGui::Text ("user in lobby:");
  for (auto &lobbyMemberAccountName : createGameLobby.accountNamesInGameLobby)
    {
      ImGui::Text (lobbyMemberAccountName.c_str ());
    }
  ImGui::PushDisabled (shouldLockScreen, time);
  createGameLobby.startGame = ImGui::Button ("Start Game", ImVec2 (-1, 0));
  createGameLobby.leaveGameLobby = ImGui::Button ("Leave Game Lobby", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen, time);
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
  ImGui::Text (messageBoxPopup.message.c_str ());
  ImGui::EndChild ();
  ImGui::EndChild ();
  ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
  for (auto &button : messageBoxPopup.buttons)
    {
      ImGui::SameLine ();
      ImGui::PushDisabled (button.disabled, time);
      button.pressed = ImGui::Button (button.name.c_str ());
      ImGui::PopDisabled (button.disabled, time);
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
  ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Sign in to XYZ").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
  ImGui::SameLine ();
  ImGui::PushFont (&biggerFont);
  ImGui::Text ("Sign in to XYZ");
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
  ImGui::Text ("Username");
  ImGui::InputText ("##username", &data.accountName);
  ImGui::Text ("Password");
  ImGui::InputText ("##password", &data.password, ImGuiInputTextFlags_Password);
  ImGui::PopDisabled (shouldLockScreen, time);
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
          ImGui::PopDisabled (shouldLockScreen, time);
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
  ImGui::Dummy (ImVec2 (((windowWidth / 2) - (ImGui::CalcTextSize ("New to XYZ?").x + ImGui::CalcTextSize ("create an account").x + (ImGui::GetStyle ().ItemSpacing.x * 6))) / 2, 0.0f));
  ImGui::SameLine ();
  ImGui::Text ("New to XYZ?");
  ImGui::SameLine ();
  data.createAccountClicked = ImGui::SmallButton ("Create an Account");
  ImGui::PopDisabled (shouldLockScreen, time);
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
  ImGui::Text ("Create your Account");
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
  ImGui::Text ("Username");
  ImGui::InputText ("##account-username", &data.accountName);
  ImGui::Text ("Password");
  ImGui::InputText ("##account-password", &data.password, ImGuiInputTextFlags_Password);
  data.backToLoginClicked = ImGui::Button ("Back");
  ImGui::PopDisabled (shouldLockScreen, time);
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
      ImGui::PopDisabled (shouldLockScreen, time);
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
  ImGui::Text ("Create Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##CreateGameLobbyName", &data.createGameLobbyName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##CreateGameLobbyPassword", &data.createGameLobbyPassword);
  data.createCreateGameLobbyClicked = ImGui::Button ("Create Game Lobby", ImVec2 (-1, 0));
  ImGui::Text ("Join Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##JoinGameLobbyName", &data.joinGameLobbyName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##JoinGameLobbyPassword", &data.joinGameLobbyPassword);
  data.createJoinGameLobbyClicked = ImGui::Button ("Join Game Lobby", ImVec2 (-1, 0));
  data.logoutButtonClicked = ImGui::Button ("Logout", ImVec2 (-1, 0));
  ImGui::PopDisabled (shouldLockScreen, time);
  ImGui::PopItemWidth ();
}

void
gameScreen (Game &game, std::optional<WaitForServer> &waitForServer, std::string const &accountName, ChatData &chatData)
{
  // TODO game screen
  ImGui::PushItemWidth (-1);
  auto const shouldLockScreen = waitForServer.has_value ();
  auto time = std::chrono::milliseconds{};
  if (waitForServer)
    {
      time = std::chrono::duration_cast<std::chrono::milliseconds> (waitForServer->elapsedTime ());
    }
  chatScreen (chatData, shouldLockScreen, time);
  ImGui::Text ("Table");
  // TODO add combobox if card is not beaten and player defends
  std::vector<bool> selectedCardsToBeat (game.gameData.table.size ());
  if (game.selectedCardFromTable)
    {
      selectedCardsToBeat.at (game.selectedCardFromTable.value ()) = true;
    }
  auto currentPlayer = std::ranges::find_if (game.gameData.players, [&accountName] (auto const &_player) { return accountName == _player.name; });
  auto currentPlayerRole = durak::PlayerRole::waiting;
  if (currentPlayer != game.gameData.players.end ())
    {
      currentPlayerRole = currentPlayer->playerRole;
    }
  for (size_t i = 0; game.gameData.table.size () > i; i++)
    {
      auto const &[card, optionalCard] = game.gameData.table.at (i);
      auto cardsOnTable = std::stringstream{};
      cardsOnTable << card << " | ";
      if (optionalCard)
        {
          cardsOnTable << optionalCard.value () << std::endl;
        }
      else
        {
          cardsOnTable << "Card: {? ,?}" << std::endl;
        }
      ImGui::Text (cardsOnTable.str ().c_str ());
      if (currentPlayerRole == durak::PlayerRole::defend && not optionalCard)
        {
          ImGui::SameLine ();
          bool tempBool = selectedCardsToBeat.at (i);
          ImGui::Checkbox (std::string{ "##defend" + std::to_string (i) }.c_str (), &tempBool);
          if (tempBool)
            {
              game.selectedCardFromTable = i;
            }
          else if (game.selectedCardFromTable && game.selectedCardFromTable == i)
            {
              game.selectedCardFromTable = {};
            }
        }
    }
  if (game.gameData.table.size ())
    {
      ImGui::Text ("Empty Table");
    }
  for (auto const &player : game.gameData.players)
    {
      auto playerName = player.name;
      ImGui::Text (playerName.c_str ());

      // for (auto const &cardOptional : player.cards)
      for (size_t i = 0; i < player.cards.size (); i++)
        {
          auto const &cardOptional = player.cards.at (i);
          if (playerName == accountName)
            {
              bool tempBool = game.selectedCards.at (i);
              ImGui::Checkbox (std::string{ "##" + std::to_string (i) }.c_str (), &tempBool);
              game.selectedCards.at (i) = tempBool;
            }
          auto card = std::string{};
          if (cardOptional)
            {
              card = fmt::format ("{}, {}", cardOptional.value ().value, magic_enum::enum_name (cardOptional.value ().type));
            }
          else
            {
              card = "Card: {? ,?}";
            }
          ImGui::SameLine ();
          ImGui::Text (card.c_str ());
        }
    }
  game.placeSelectedCardsOnTable = ImGui::Button ("Place selected Cards on Table", ImVec2 (-1, 0));
  ImGui::PopItemWidth ();
}
