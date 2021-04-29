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
#include <filesystem>
#include <game_01_shared_class/serialization.hxx>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <memory>
#include <misc/cpp/imgui_stdlib.h>
#include <variant>
template <class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};
template <class... Ts> overloaded (Ts...) -> overloaded<Ts...>;

void
createAccountErrorPopup (float windowSizeX, float windowSizeY)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("createAccountErrorPopup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  if (ImGui::BeginPopup ("createAccountErrorPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Create Account Error").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      // ImGui::PushFont (font2);
      ImGui::Text ("Create Account Error");
      // ImGui::PopFont ();
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
        }
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
}

GuiState
TestStateMachine::login (Login &loginState, float windowSizeX, float windowSizeY)
{
  if (WebserviceController::hasLoginState ())
    {
      if (WebserviceController::isLoggedIn ())
        {
          return Lobby{};
        }
    }
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::SetNextWindowSize (ImVec2 (windowWidth, windowHeight));
  ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
  ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Sign in to XYZ").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
  ImGui::SameLine ();
  // ImGui::PushFont (font2);
  ImGui::Text ("Sign in to XYZ");
  // ImGui::PopFont ();
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
  if (ImGui::Button ("Sign in", ImVec2 (-1, 0)))
    {
      std::cout << "Sign in" << std::endl;
      if (not loginState.username.empty () && not loginState.password.empty ())
        {
          WebserviceController::sendObject (shared_class::LoginAccount{ .accountName = loginState.username, .password = loginState.password });
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
  loginState.shouldOpenCreateAnAccount = loginState.shouldOpenCreateAnAccount || ImGui::SmallButton ("create an account");
  ImGui::PopStyleVar ();
  ImGui::EndChild ();
  // TODO think about this create account error popup should be come from CreateAccount state i think
  if (loginState.shouldOpenCreateAnAccount)
    {
      if (WebserviceController::isCreateAccountError ())
        {
          createAccountErrorPopup (windowWidth, windowHeight);
        }
      else
        {
          return CreateAccount{};
        }
    }
  return loginState;
}

GuiState
TestStateMachine::createAccountPopup (CreateAccount &createAccountState, float windowSizeX, float windowSizeY)
{
  if (WebserviceController::hasLoginState ())
    {
      if (WebserviceController::isLoggedIn ())
        {
          return Lobby{};
        }
    }
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
      // ImGui::PushFont (font2);
      ImGui::Text ("Create your account");
      // ImGui::PopFont ();
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
          ImGui::CloseCurrentPopup ();
          backButtonPressed = true;
        }
      ImGui::SameLine ();
      if (ImGui::Button ("Create account"))
        {
          {
            if (not createAccountState.username.empty () && not createAccountState.password.empty ())
              {
                WebserviceController::sendObject (shared_class::CreateAccount{ .accountName = createAccountState.username, .password = createAccountState.password });
              }
          }
        }
      ImGui::PopItemWidth ();
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
  // TODO try to refactor this there should be a better way to do this
  if (backButtonPressed)
    {
      return Login{};
    }
  else
    {
      return createAccountState;
    }
}

GuiState
TestStateMachine::lobby (Lobby &lobbyState, float windowSizeX, float windowSizeY)
{
}

void
TestStateMachine::execute (float windowSizeX, float windowSizeY)
{

  auto visit = overloaded{
    [&] (Login &arg) {
      // std::cout << "Login" << std::endl;
      guiState = login (arg, windowSizeX, windowSizeY);
    },

    [&] (CreateAccount &arg) {
      // std::cout << "CreateAccount" << std::endl;
      guiState = createAccountPopup (arg, windowSizeX, windowSizeY);
    },

    [&] (Lobby &arg) {
      // std::cout << "Lobby" << std::endl;
      guiState = lobby (arg, windowSizeX, windowSizeY);
    },
  };
  std::visit (visit, guiState);
}
