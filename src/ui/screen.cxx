#include "src/ui/screen.hxx"
#include "src/controller/state.hxx"
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
void
loginScreen (Login &login, float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::SetNextWindowSize (ImVec2 (windowWidth, windowHeight));
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
  ImGui::InputText ("##username", &login.accountName);
  ImGui::Text ("Password");
  ImGui::InputText ("##password", &login.password, ImGuiInputTextFlags_Password);
  login.loginClicked = ImGui::Button ("Sign in", ImVec2 (-1, 0));
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
  login.createAccountClicked = ImGui::SmallButton ("create an account");
  ImGui::PopStyleVar ();
  ImGui::EndChild ();
}

void
createAccountScreen (CreateAccount &createAccount, float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("my_select_popup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Create your account").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text ("Create your account");
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
      ImGui::InputText ("##account-username", &createAccount.accountName);
      ImGui::Text ("Password");
      ImGui::InputText ("##account-password", &createAccount.password, ImGuiInputTextFlags_Password);
      createAccount.backToLoginClicked = ImGui::Button ("Back");
      ImGui::SameLine ();
      createAccount.createAccountClicked = ImGui::Button ("Create account");
      ImGui::PopItemWidth ();
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
}

// void
// waitForServerPopup (WaitForServerPopupState &waitForServerPopupState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
// {
//   auto const windowWidth = windowSizeX;
//   auto const windowHeight = windowSizeY;
//   ImGui::OpenPopup ("my_select_popup");
//   ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
//   if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
//     {
//       ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
//       ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
//       ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
//       ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("WaitForServerPopupState").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
//       ImGui::SameLine ();
//       ImGui::PushFont (&biggerFont);
//       ImGui::Text ("WaitForServerPopupState");
//       ImGui::PopFont ();
//       ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
//       ImGui::SameLine ();
//       ImGui::BeginChild ("ChildR", ImVec2 (windowWidth / 2, (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 40), true, window_flags);
//       ImGui::Dummy (ImVec2 (0.0f, 10.0f));
//       ImGui::Dummy (ImVec2 (10.0f, 0.0f));
//       ImGui::SameLine ();
//       ImGui::PushStyleVar (ImGuiStyleVar_ChildBorderSize, 0);
//       ImGui::BeginChild ("ChildR_sub", ImVec2 ((windowWidth / 2) - 50, (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 10), true, window_flags);
//       ImGui::PopStyleVar ();
//       ImGui::Text ("Wait For Server");
//       ImGui::EndChild ();
//       ImGui::EndChild ();
//       ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
//       ImGui::SameLine ();
//       waitForServerPopupState.cancelButtonClicked = ImGui::Button ("Cancel");
//       ImGui::PopStyleVar ();
//       ImGui::EndPopup ();
//     }
// }

// void
// messageBoxPopup (MessageBoxPopupState &messageBoxPopupState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
// {
//   auto const windowWidth = windowSizeX;
//   auto const windowHeight = windowSizeY;
//   ImGui::OpenPopup ("my_select_popup");
//   ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
//   if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
//     {
//       ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
//       ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
//       ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
//       ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("WaitForServerPopupState").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
//       ImGui::SameLine ();
//       ImGui::PushFont (&biggerFont);
//       ImGui::Text ("WaitForServerPopupState");
//       ImGui::PopFont ();
//       ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
//       ImGui::SameLine ();
//       ImGui::BeginChild ("ChildR", ImVec2 (windowWidth / 2, (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 40), true, window_flags);
//       ImGui::Dummy (ImVec2 (0.0f, 10.0f));
//       ImGui::Dummy (ImVec2 (10.0f, 0.0f));
//       ImGui::SameLine ();
//       ImGui::PushStyleVar (ImGuiStyleVar_ChildBorderSize, 0);
//       ImGui::BeginChild ("ChildR_sub", ImVec2 ((windowWidth / 2) - 50, (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2)) + 10), true, window_flags);
//       ImGui::PopStyleVar ();
//       ImGui::Text ("Wait For Server");
//       ImGui::EndChild ();
//       ImGui::EndChild ();
//       ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
//       for (auto &button : messageBoxPopupState.buttons)
//         {
//           ImGui::SameLine ();
//           button.second = ImGui::Button (button.first.c_str ());
//         }
//       ImGui::PopStyleVar ();
//       ImGui::EndPopup ();
//     }
// }

void
chatScreen (ChatData &chatData)
{
  ImGui::Text ("Join Channel");
  ImGui::InputText ("##JoinChannel", &chatData.channelToJoin);
  chatData.joinChannelClicked = ImGui::Button ("Join Channel", ImVec2 (-1, 0));
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

  ImGui::Text ("Send to Channel");
  ImGui::InputText ("##SendToChannel", &chatData.messageToSendToChannel);
  chatData.sendMessageClicked = ImGui::Button ("Send to Channel", ImVec2 (-1, 0));
}

void
lobbyScreen (Lobby &lobby, ImFont &, ChatData &chatData)
{
  chatScreen (chatData);
  // TODO allow joinin a game
  ImGui::Text ("Create Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##CreateGameLobbyName", &lobby.createGameLobbyName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##CreateGameLobbyPassword", &lobby.createGameLobbyPassword);
  lobby.createCreateGameLobbyClicked = ImGui::Button ("Create Game Lobby", ImVec2 (-1, 0));
  ImGui::Text ("Join Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##JoinGameLobbyName", &lobby.joinGameLobbyName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##JoinGameLobbyPassword", &lobby.joinGameLobbyPassword);
  lobby.createJoinGameLobbyClicked = ImGui::Button ("Join Game Lobby", ImVec2 (-1, 0));
  lobby.logoutButtonClicked = ImGui::Button ("Logout", ImVec2 (-1, 0));
}

void
createGameLobbyScreen (CreateGameLobby &createGameLobby, ChatData &chatData)
{
  chatScreen (chatData);
  ImGui::Text (std::string{ "max user count: " + std::to_string (createGameLobby.maxUserInGameLobbyToSend) }.c_str ());
  if (createGameLobby.accountName == createGameLobby.accountNamesInGameLobby.at (0))
    {
      ImGui::Text ("set max user count: ");
      ImGui::SameLine ();
      ImGui::InputInt ("##MaxUserCount", &createGameLobby.maxUserInGameLobby);
      createGameLobby.sendMaxUserCountClicked = ImGui::Button ("set max user count", ImVec2 (-1, 0));
    }
  for (auto &accountName : createGameLobby.accountNamesInGameLobby)
    {
      ImGui::Text (accountName.c_str ());
    }
  createGameLobby.sendMaxUserCountClicked = ImGui::Button ("Start Game", ImVec2 (-1, 0));
  createGameLobby.leaveGameLobby = ImGui::Button ("Leave Game Lobby", ImVec2 (-1, 0));
}