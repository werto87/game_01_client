#include "src/ui/uiState.hxx"
#include "src/controller/database.hxx"
#include "src/controller/webservice.hxx"
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData.h>
#include <algorithm>
#include <exception>
#include <filesystem>
#include <game_01_shared_class/serialization.hxx>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <memory>
#include <misc/cpp/imgui_stdlib.h>
#include <string>
#include <variant>
template <class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};
template <class... Ts> overloaded (Ts...) -> overloaded<Ts...>;

GuiState
createAccountSuccessPopup (float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("createAccountSuccessPopup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  auto backButtonPressed = false;
  if (ImGui::BeginPopup ("createAccountSuccessPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Create Account Success").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text ("Create Account Success");
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
      ImGui::Text ("%s", "Create Account Success");
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
      ImGui::SameLine ();
      if (ImGui::Button ("Back"))
        {
          WebserviceController::resetSession ();
          ImGui::CloseCurrentPopup ();
          backButtonPressed = true;
        }
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
  if (backButtonPressed)
    {
      return CreateAccount{};
    }
  else
    {
      return CreateAccountSuccess{};
    }
}

GuiState
createAccountErrorPopup (float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("createAccountErrorPopup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  auto backButtonPressed = false;
  if (ImGui::BeginPopup ("createAccountErrorPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Create Account Error").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text ("Create Account Error");
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
      ImGui::Text ("%s", WebserviceController::createAccountError ().c_str ());
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
      ImGui::SameLine ();
      if (ImGui::Button ("Back"))
        {
          WebserviceController::resetSession ();
          ImGui::CloseCurrentPopup ();
          backButtonPressed = true;
        }
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
  if (backButtonPressed)
    {
      return CreateAccount{};
    }
  else
    {
      return CreateAccountError{};
    }
}

GuiState
login (Login &loginState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  if (WebserviceController::hasLoginState ())
    {
      if (WebserviceController::isLoggedIn ())
        {
          return Lobby{};
        }
      else
        {
          return LoginError{};
        }
    }
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
  ImGui::InputText ("##username", &loginState.username);
  ImGui::Text ("Password");
  ImGui::InputText ("##password", &loginState.password, ImGuiInputTextFlags_Password);
  // TODO if user puts in name and clicks the sign in button. button gets grey but no message gets send
  // TODO check this for all usages of disable button
  if (loginState.loginInProgress)
    {
      ImGui::PushItemFlag (ImGuiItemFlags_Disabled, true);
      ImGui::PushStyleVar (ImGuiStyleVar_Alpha, ImGui::GetStyle ().Alpha * 0.5f);
      ImGui::Button ("Sign in", ImVec2 (-1, 0));
      ImGui::PopItemFlag ();
      ImGui::PopStyleVar ();
    }
  else
    {
      if (ImGui::Button ("Sign in", ImVec2 (-1, 0)))
        {
          loginState.loginInProgress = true;
          std::cout << "Sign in" << std::endl;
          if (not loginState.username.empty () && not loginState.password.empty ())
            {
              WebserviceController::sendObject (shared_class::LoginAccount{ .accountName = loginState.username, .password = loginState.password });
            }
        }
    }
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
  auto shouldOpenCreateAnAccount = false;
  if (loginState.loginInProgress)
    {
      ImGui::PushItemFlag (ImGuiItemFlags_Disabled, true);
      ImGui::PushStyleVar (ImGuiStyleVar_Alpha, ImGui::GetStyle ().Alpha * 0.5f);
      ImGui::SmallButton ("create an account");
      ImGui::PopItemFlag ();
      ImGui::PopStyleVar ();
    }
  else
    {
      shouldOpenCreateAnAccount = ImGui::SmallButton ("create an account");
    }
  ImGui::PopStyleVar ();
  ImGui::EndChild ();
  if (shouldOpenCreateAnAccount)
    {
      return CreateAccount{};
    }
  return loginState;
}

GuiState
createAccountPopup (CreateAccount &createAccountState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  if (WebserviceController::hasAccountCreateSuccessState ())
    {
      if (WebserviceController::isAccountCreateSuccess ())
        {
          return CreateAccountSuccess{};
        }
      else
        {
          return CreateAccountError{};
        }
    }
  else
    {
      auto const windowWidth = windowSizeX;
      auto const windowHeight = windowSizeY;
      ImGui::OpenPopup ("my_select_popup");
      ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
      auto backButtonPressed = false;
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
          ImGui::InputText ("##account-username", &createAccountState.username);
          ImGui::Text ("Password");
          ImGui::InputText ("##account-password", &createAccountState.password, ImGuiInputTextFlags_Password);
          if (ImGui::Button ("Back"))
            {
              WebserviceController::resetSession ();
              ImGui::CloseCurrentPopup ();
              backButtonPressed = true;
            }
          ImGui::SameLine ();

          if (createAccountState.createAccountInProgress)
            {
              ImGui::PushItemFlag (ImGuiItemFlags_Disabled, true);
              ImGui::PushStyleVar (ImGuiStyleVar_Alpha, ImGui::GetStyle ().Alpha * 0.5f);
              ImGui::Button ("Create account");
              ImGui::PopItemFlag ();
              ImGui::PopStyleVar ();
            }
          else
            {
              if (ImGui::Button ("Create account"))
                {
                  {
                    createAccountState.createAccountInProgress = true;
                    if (not createAccountState.username.empty () && not createAccountState.password.empty ())
                      {
                        WebserviceController::sendObject (shared_class::CreateAccount{ .accountName = createAccountState.username, .password = createAccountState.password });
                      }
                  }
                }
            }

          ImGui::PopItemWidth ();
          ImGui::EndChild ();
          ImGui::EndChild ();
          ImGui::PopStyleVar ();
          ImGui::EndPopup ();
        }
      if (backButtonPressed)
        {
          return Login{};
        }
      else
        {
          return createAccountState;
        }
    }
}

GuiState
lobby (Lobby &lobbyState, ImFont &)
{
  if (not(WebserviceController::hasLoginState () && WebserviceController::isLoggedIn ()))
    {
      return Login{};
    }
  if (WebserviceController::hasCreateGameLobbyName ())
    {
      return LobbyForCreatingAGame{};
    }
  if (WebserviceController::hasRelogToDestination ())
    {
      return WantToRelogPopup{};
    }
  if (WebserviceController::hasRelogToError ())
    {
      return RelogToError{};
    }
  auto channelNames = WebserviceController::channelNames ();
  if (ImGui::BeginCombo ("combo 1", lobbyState.selectedChannelName ? lobbyState.selectedChannelName.value ().data () : "Select Channel"))
    {
      if ((not lobbyState.selectedChannelName && not channelNames.empty ()) || (lobbyState.selectedChannelName && not channelNames.empty () && std::ranges::find (channelNames, lobbyState.selectedChannelName) == channelNames.end ()))
        {
          lobbyState.selectedChannelName = channelNames.front ();
        }
      for (auto &&channelName : channelNames)
        {
          const bool is_selected = (lobbyState.selectedChannelName == channelName);
          if (ImGui::Selectable (channelName.data (), is_selected)) lobbyState.selectedChannelName = channelName;
          if (is_selected) ImGui::SetItemDefaultFocus ();
        }
      ImGui::EndCombo ();
    }
  ImGui::BeginChild ("scrolling", ImVec2 (0, 500), false, ImGuiWindowFlags_HorizontalScrollbar);
  if (lobbyState.selectedChannelName && std::ranges::find (channelNames, lobbyState.selectedChannelName) != channelNames.end ())
    {
      for (auto text : WebserviceController::messagesForChannel (lobbyState.selectedChannelName.value ()))
        {
          ImGui::TextUnformatted (text.data (), text.data () + text.size ());
        }
      if (ImGui::GetScrollY () >= ImGui::GetScrollMaxY ()) ImGui::SetScrollHereY (1.0f);
    }
  ImGui::EndChild ();
  ImGui::Text ("Join Channel");
  ImGui::InputText ("##JoinChannel", &lobbyState.channelToJoin);
  if (ImGui::Button ("Join Channel", ImVec2 (-1, 0)))
    {
      if (not lobbyState.channelToJoin.empty ())
        {
          WebserviceController::sendObject (shared_class::JoinChannel{ .channel = lobbyState.channelToJoin });
          lobbyState.channelToJoin.clear ();
        }
    }
  ImGui::Text ("Send to Channel");
  ImGui::InputText ("##SendToChannel", &lobbyState.messageToSendToChannel);
  if (ImGui::Button ("Send to Channel", ImVec2 (-1, 0)))
    {
      if (lobbyState.selectedChannelName && not lobbyState.selectedChannelName->empty () && not lobbyState.messageToSendToChannel.empty ())
        {
          WebserviceController::sendObject (shared_class::BroadCastMessage{ .channel = lobbyState.selectedChannelName.value (), .message = lobbyState.messageToSendToChannel });
          lobbyState.messageToSendToChannel.clear ();
        }
    }
  // TODO create the game lobby and allow joinin a game

  ImGui::Text ("Create Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##CreateGameLobbyName", &lobbyState.gameLobbyToCreateName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##CreateGameLobbyPassword", &lobbyState.gameLobbyToCreatePassword);
  if (ImGui::Button ("Create Game Lobby", ImVec2 (-1, 0)))
    {
      if (not lobbyState.gameLobbyToCreateName.empty ())
        {
          WebserviceController::sendObject (shared_class::CreateGameLobby{ .name = lobbyState.gameLobbyToCreateName, .password = lobbyState.gameLobbyToCreatePassword });
          lobbyState.gameLobbyToCreateName.clear ();
          lobbyState.gameLobbyToCreatePassword.clear ();
        }
    }

  ImGui::Text ("Join Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##JoinGameLobbyName", &lobbyState.gameLobbyToJoinName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##JoinGameLobbyPassword", &lobbyState.gameLobbyToJoinPassword);
  if (ImGui::Button ("Join Game Lobby", ImVec2 (-1, 0)))
    {
      if (not lobbyState.gameLobbyToJoinName.empty ())
        {
          WebserviceController::sendObject (shared_class::JoinGameLobby{ .name = lobbyState.gameLobbyToJoinName, .password = lobbyState.gameLobbyToJoinPassword });
          lobbyState.gameLobbyToJoinName.clear ();
          lobbyState.gameLobbyToJoinPassword.clear ();
        }
    }

  if (ImGui::Button ("Join Game", ImVec2 (-1, 0)))
    {
      std::cout << "JOIN GAME LOBBY" << std::endl;
    }
  if (ImGui::Button ("Logout", ImVec2 (-1, 0)))
    {
      WebserviceController::sendObject (shared_class::LogoutAccount{});
    }
  return lobbyState;
}

GuiState
loginErrorPopup (float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("my_select_popup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  auto closePopup = false;
  if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Login error").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text ("Login error");
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
      ImGui::Text ("%s", WebserviceController::logInMessageFromServer ().c_str ());
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
      ImGui::SameLine ();
      if (ImGui::Button ("Back"))
        {
          WebserviceController::resetSession ();
          ImGui::CloseCurrentPopup ();
          closePopup = true;
        }
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
  if (closePopup)
    {
      return Login{};
    }
  else
    {
      return LoginError{};
    }
}

GuiState
wantToRelogPopup (float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("my_select_popup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  auto closePopup = false;
  if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      auto headline = std::string{ "Want to relog to " + WebserviceController::relogToDestination () + "?" };
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize (headline.c_str ()).x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text (headline.c_str ());
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
      if (ImGui::Button ("do not go to destination"))
        {
          WebserviceController::removeRelogToDestinationMessage ();
          WebserviceController::sendObject (shared_class::RelogTo{ .wantsToRelog = false });
          ImGui::CloseCurrentPopup ();
          closePopup = true;
        }
      ImGui::SameLine ();
      if (ImGui::Button ("go to destination"))
        {
          WebserviceController::removeRelogToDestinationMessage ();
          WebserviceController::sendObject (shared_class::RelogTo{ .wantsToRelog = true });
          ImGui::CloseCurrentPopup ();
          closePopup = true;
        }
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
      ImGui::SameLine ();

      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
  if (closePopup)
    {
      return Lobby{};
    }
  else
    {
      return WantToRelogPopup{};
    }
}

GuiState
relogToErrorPopup (float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("my_select_popup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  auto closePopup = false;
  if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      auto headline = WebserviceController::relogToError ();
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize (headline.c_str ()).x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text (headline.c_str ());
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
      if (ImGui::Button ("ok"))
        {
          WebserviceController::removeRelogToError ();
          ImGui::CloseCurrentPopup ();
          closePopup = true;
        }
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
      ImGui::SameLine ();

      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
  if (closePopup)
    {
      return Lobby{};
    }
  else
    {
      return WantToRelogPopup{};
    }
}

GuiState
lobbyForCreatingAGame (LobbyForCreatingAGame &createGameLobbyState, ImFont &)
{
  if (not WebserviceController::hasCreateGameLobbyName ())
    {
      return Lobby{};
    }
  else
    {
      ImGui::Text (std::string{ "max user count: " + std::to_string (WebserviceController::getMaxUsersInGameLobby ()) }.c_str ());
      if (WebserviceController::getAccountName () == WebserviceController::accountNamesInCreateGameLobby ().at (0))
        {
          ImGui::Text ("set max user count: ");
          ImGui::SameLine ();
          ImGui::InputInt ("##MaxUserCount", &createGameLobbyState.setMaxAccountInGameLobby);
          if (ImGui::Button ("set max user count", ImVec2 (-1, 0)))
            {
              WebserviceController::sendObject (shared_class::SetMaxUserSizeInCreateGameLobby{ .createGameLobbyName = WebserviceController::createGameLobbyName (), .maxUserSize = static_cast<size_t> (createGameLobbyState.setMaxAccountInGameLobby) });
            }
        }
      for (auto &accountName : WebserviceController::accountNamesInCreateGameLobby ())
        {
          ImGui::Text (accountName.c_str ());
        }
      if (ImGui::Button ("leave game lobby", ImVec2 (-1, 0)))
        {
          WebserviceController::sendObject (shared_class::LeaveGameLobby{});
        }
      return createGameLobbyState;
    }
}

void
UiState::execute (float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto visit = overloaded{
    [&] (Login &arg) {
      // std::cout << "Login" << std::endl;
      guiState = login (arg, windowSizeX, windowSizeY, biggerFont);
    },
    [&] (LoginError &) {
      // std::cout << "LoginError" << std::endl;
      guiState = loginErrorPopup (windowSizeX, windowSizeY, biggerFont);
    },
    [&] (CreateAccount &arg) {
      // std::cout << "CreateAccount" << std::endl;
      guiState = createAccountPopup (arg, windowSizeX, windowSizeY, biggerFont);
    },
    [&] (CreateAccountSuccess &) {
      // std::cout << "CreateAccountSuccess" << std::endl;
      guiState = createAccountSuccessPopup (windowSizeX, windowSizeY, biggerFont);
    },
    [&] (CreateAccountError &) {
      // std::cout << "CreateAccountError" << std::endl;
      guiState = createAccountErrorPopup (windowSizeX, windowSizeY, biggerFont);
    },
    [&] (Lobby &arg) {
      // std::cout << "Lobby" << std::endl;
      guiState = lobby (arg, biggerFont);
    },
    [&] (LobbyForCreatingAGame &arg) {
      // std::cout << "lobbyForCreatingAGame" << std::endl;
      guiState = lobbyForCreatingAGame (arg, biggerFont);
    },
    [&] (WantToRelogPopup &) {
      // std::cout << "WantToRelogPopup" << std::endl;
      guiState = wantToRelogPopup (windowSizeX, windowSizeY, biggerFont);
    },
    [&] (RelogToError &) {
      // std::cout << "RelogToError" << std::endl;
      guiState = relogToErrorPopup (windowSizeX, windowSizeY, biggerFont);
    },

  };
  std::visit (visit, guiState);
}
