#ifndef B7441788_8126_4373_8FB3_72A5D223C8CB
#define B7441788_8126_4373_8FB3_72A5D223C8CB

#include "imgui.h"
#include "src/controller/commonEvent.hxx"
#include "src/controller/loginStateMachineState.hxx"
#include "src/controller/makeGameMachineState.hxx"
#include "src/controller/stateMachineData.hxx"
#include <algorithm>
#include <misc/cpp/imgui_stdlib.h>
#include <ranges>
#include <type_traits>
#include <variant>

namespace ImGui
{
inline void
PushDisabled ()
{
  extern void PushItemFlag (int option, bool enabled);
  PushItemFlag (1 << 2 /*ImGuiItemFlags_Disabled*/, !false);
  PushStyleVar (ImGuiStyleVar_Alpha, ImGui::GetStyle ().Alpha * (false ? 1.0f : 0.5f));
}

inline void
PopDisabled ()
{
  extern void PopItemFlag ();
  PopItemFlag ();
  PopStyleVar ();
}

} // namespace ImGui

void chatScreen (ChatData &chatData);
void createGameLobbyScreen (CreateGameLobby &createGameLobby, std::string accountName, ChatData &chatData);

template <typename T> concept hasPopUpData = requires { T{}.message; };

// TODO find a solution where we can seperate function into declaration and definition so we dont need to include imgui in statemachine

void messageBoxPopupScreen (MessageBoxPopup &messageBoxPopup, float windowWidth, float windowHeight, ImFont &biggerFont);

template <typename T>
void
loginScreen (T &data, float windowWidth, float windowHeight, ImFont &biggerFont)
{
  constexpr auto isLogin = std::is_same<T, Login>::value;
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
  if constexpr (isLogin)
    {
      ImGui::InputText ("##username", &data.accountName);
    }
  else
    {
      auto placeholder = std::string{};
      ImGui::PushDisabled ();
      ImGui::InputText ("##username", &placeholder);
      ImGui::PopDisabled ();
    }
  ImGui::Text ("Password");
  if constexpr (isLogin)
    {
      ImGui::InputText ("##password", &data.password, ImGuiInputTextFlags_Password);
    }
  else
    {
      auto placeholder = std::string{};
      ImGui::PushDisabled ();
      ImGui::InputText ("##password", &placeholder, ImGuiInputTextFlags_ReadOnly);
      ImGui::PopDisabled ();
    }
  if constexpr (isLogin)
    {
      data.loginClicked = ImGui::Button ("Sign in", ImVec2 (-1, 0));
    }
  else
    {
      auto &[buttonText, buttonVaule] = data.buttons.front ();
      using namespace std::chrono_literals;
      if (data.elapsedTime () >= 5s)
        {
          buttonVaule = ImGui::Button (buttonText.c_str (), ImVec2 (-1, 0));
        }
      else
        {
          ImGui::PushDisabled ();
          buttonVaule = ImGui::Button (buttonText.c_str (), ImVec2 (-1, 0));
          ImGui::PopDisabled ();
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
  if constexpr (isLogin)
    {
      data.createAccountClicked = ImGui::SmallButton ("Create an Account");
    }
  else
    {
      ImGui::PushDisabled ();
      ImGui::SmallButton ("Create an Account");
      ImGui::PopDisabled ();
    }

  ImGui::PopStyleVar ();
  ImGui::EndChild ();
}

template <typename T>
void
createAccountScreen (T &data, float windowWidth, float windowHeight, ImFont &biggerFont)
{
  constexpr auto isCreateAccount = std::is_same<T, CreateAccount>::value;
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
  if constexpr (isCreateAccount)
    {
      ImGui::InputText ("##account-username", &data.accountName);
    }
  else
    {
      auto placeholder = std::string{};
      ImGui::PushDisabled ();
      ImGui::InputText ("##account-username", &placeholder);
      ImGui::PopDisabled ();
    }

  ImGui::Text ("Password");
  if constexpr (isCreateAccount)
    {
      ImGui::InputText ("##account-password", &data.password, ImGuiInputTextFlags_Password);
    }
  else
    {
      auto placeholder = std::string{};
      ImGui::PushDisabled ();
      ImGui::InputText ("##account-password", &placeholder);
      ImGui::PopDisabled ();
    }
  if constexpr (isCreateAccount)
    {
      data.backToLoginClicked = ImGui::Button ("Back");
    }
  else
    {
      auto &[buttonText, buttonVaule] = data.buttons.front ();
      using namespace std::chrono_literals;
      if (data.elapsedTime () >= 5s)
        {
          buttonVaule = ImGui::Button (buttonText.c_str ());
        }
      else
        {
          ImGui::PushDisabled ();
          buttonVaule = ImGui::Button (buttonText.c_str ());
          ImGui::PopDisabled ();
        }
    }
  ImGui::SameLine ();
  if constexpr (isCreateAccount)
    {
      data.createAccountClicked = ImGui::Button ("Create Account and Sign in");
    }
  else
    {
      auto &[buttonText, buttonVaule] = data.buttons.at (1);
      ImGui::PushDisabled ();
      buttonVaule = ImGui::Button (buttonText.c_str ());
      ImGui::PopDisabled ();
    }
  ImGui::PopItemWidth ();
  ImGui::EndChild ();
  ImGui::EndChild ();
  ImGui::PopStyleVar ();
}

template <typename T>
void
lobbyScreen (T &data, ChatData &chatData)
{
  constexpr auto isLobby = std::is_same<T, Lobby>::value;
  chatScreen (chatData);
  ImGui::Text ("Create Game Lobby");
  ImGui::Text ("Game Lobby Name");
  if constexpr (isLobby)
    {
      ImGui::InputText ("##CreateGameLobbyName", &data.createGameLobbyName);
    }
  else
    {
      ImGui::PushDisabled ();
      auto placeholder = std::string{};
      ImGui::InputText ("##CreateGameLobbyName", &placeholder);
      ImGui::PopDisabled ();
    }
  ImGui::Text ("Game Lobby Password");
  if constexpr (isLobby)
    {
      ImGui::InputText ("##CreateGameLobbyPassword", &data.createGameLobbyPassword);
    }
  else
    {
      ImGui::PushDisabled ();
      auto placeholder = std::string{};
      ImGui::InputText ("##CreateGameLobbyPassword", &placeholder);
      ImGui::PopDisabled ();
    }
  if constexpr (isLobby)
    {
      data.createCreateGameLobbyClicked = ImGui::Button ("Create Game Lobby", ImVec2 (-1, 0));
    }
  else
    {
      ImGui::PushDisabled ();
      ImGui::Button ("Create Game Lobby", ImVec2 (-1, 0));
      ImGui::PopDisabled ();
    }
  ImGui::Text ("Join Game Lobby");
  ImGui::Text ("Game Lobby Name");
  if constexpr (isLobby)
    {
      ImGui::InputText ("##JoinGameLobbyName", &data.joinGameLobbyName);
    }
  else
    {
      ImGui::PushDisabled ();
      auto placeholder = std::string{};
      ImGui::InputText ("##JoinGameLobbyName", &placeholder);
      ImGui::PopDisabled ();
    }
  ImGui::Text ("Game Lobby Password");
  if constexpr (isLobby)
    {
      ImGui::InputText ("##JoinGameLobbyPassword", &data.joinGameLobbyPassword);
    }
  else
    {
      ImGui::PushDisabled ();
      auto placeholder = std::string{};
      ImGui::InputText ("##JoinGameLobbyPassword", &placeholder);
      ImGui::PopDisabled ();
    }
  if constexpr (isLobby)
    {
      data.createJoinGameLobbyClicked = ImGui::Button ("Join Game Lobby", ImVec2 (-1, 0));
    }
  else
    {
      ImGui::PushDisabled ();
      ImGui::Button ("Join Game Lobby", ImVec2 (-1, 0));
      ImGui::PopDisabled ();
    }
  if constexpr (isLobby)
    {
      data.logoutButtonClicked = ImGui::Button ("Logout", ImVec2 (-1, 0));
    }
  else
    {
      ImGui::PushDisabled ();
      ImGui::Button ("Logout", ImVec2 (-1, 0));
      ImGui::PopDisabled ();
    }
}

const auto drawLogin = [] (draw const &drawEv, Login &login) { loginScreen (login, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont); };
const auto drawLoginWaitForServer = [] (draw const &drawEv, LoginWaitForServer &loginWaitForServer, MessageBoxPopup &messageBoxPopup) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      loginScreen (loginWaitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
};

const auto drawLoginError = [] (draw const &, LoginError &) {};
const auto drawCreateAccount = [] (draw const &drawEv, CreateAccount &createAccount) { createAccountScreen (createAccount, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont); };
const auto drawCreateAccountWaitForServer = [] (draw const &drawEv, CreateAccountWaitForServer &createAccountWaitForServer, MessageBoxPopup &messageBoxPopup) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      createAccountScreen (createAccountWaitForServer, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
};
const auto drawCreateAccountError = [] (draw const &, CreateAccountError &) {};
const auto drawCreateAccountSuccess = [] (draw const &, CreateAccountSuccess &) {};

const auto drawLobby = [] (draw const &, Lobby &lobby, MakeGameMachineData &makeGameMachineData) { lobbyScreen (lobby, makeGameMachineData.chatData); };
const auto drawCreateGameLobbyWaitForServer = [] (draw const &drawEv, CreateGameLobbyWaitForServer &createGameLobbyWaitForServer, MakeGameMachineData &makeGameMachineData, MessageBoxPopup &messageBoxPopup) {
  if (not std::holds_alternative<std::monostate> (messageBoxPopup.event))
    {
      messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont);
    }
  else
    {
      lobbyScreen (createGameLobbyWaitForServer, makeGameMachineData.chatData);
    }
};
const auto drawCreateGameLobbyError = [] (draw const &, CreateGameLobbyError &) {};
const auto drawCreateGameLobby = [] (draw const &, CreateGameLobby &createGameLobby, MakeGameMachineData &makeGameMachineData) { createGameLobbyScreen (createGameLobby, makeGameMachineData.accountName, makeGameMachineData.chatData); };

const auto showMessageBoxPopup = [] (draw const &drawEv, MessageBoxPopup &messageBoxPopup) { messageBoxPopupScreen (messageBoxPopup, drawEv.windowSizeX, drawEv.windowSizeY, *drawEv.biggerFont); };

const auto evalMessageBoxPopup = [] (MessageBoxPopup &messageBoxPopup) {
  // TODO std::visit messageBoxPopup.event
};

#endif /* B7441788_8126_4373_8FB3_72A5D223C8CB */
