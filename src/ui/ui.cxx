#include "src/ui/ui.hxx"
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

using namespace Magnum;
ImGuiExample::ImGuiExample (const Arguments &arguments) : Magnum::Platform::Application{ arguments, Configuration{}.setTitle ("Magnum ImGui Example").setWindowFlags (Configuration::WindowFlag::Resizable).setSize (Vector2i{ 800, 600 }, Vector2{ 1, 1 }) }
{
  ImGui::CreateContext ();
  ImGuiIO &io = ImGui::GetIO ();
  static auto iniFile = std::filesystem::path{ PATH_TO_BINARY }.parent_path ().append ("asset").append ("imgui.ini");
  io.IniFilename = iniFile.c_str ();
  std::cout << io.IniFilename << std::endl;
  io.Fonts->AddFontFromFileTTF ("/usr/share/fonts/TTF/SourceSansPro-Regular.ttf", 50.0f * _fontScale);
  _imgui = Magnum::ImGuiIntegration::Context (*ImGui::GetCurrentContext (), windowSize ());
  font2 = io.Fonts->AddFontFromFileTTF ("/usr/share/fonts/TTF/SourceSansPro-Regular.ttf", 75 * _fontScale);
  /* Set up proper blending to be used by ImGui. There's a great chance
     you'll need this exact behavior for the rest of your scene. If not, set
     this only for the drawFrame() call. */
  Magnum::GL::Renderer::setBlendEquation (GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
  Magnum::GL::Renderer::setBlendFunction (GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
  ImGuiStyle &style = ImGui::GetStyle ();
  style.Colors[ImGuiCol_Text] = { 1, 1, 1, 1 };
  style.Colors[ImGuiCol_PopupBg] = { 0, 0, 0, 1 };
  style.Colors[ImGuiCol_WindowBg] = { 0, 0, 0, 1 };
}

void
ImGuiExample::debug (bool &shouldChangeFontSize)
{
  ImGui::Dummy (ImVec2 (0.0f, static_cast<float> (windowSize ().y ()) / 4));
  ImGui::Text ("Message to Send");
  ImGui::InputText ("##sendMessage", &sendMessage);
  if (ImGui::Button ("Send", ImVec2 (-1, 0)))
    {
      if (not sendMessage.empty ())
        {
          WebserviceController::sendMessage (sendMessage);
        }
    }
  ImGui::SliderFloat ("Scale Font", &_fontScale, 0.1f, 1.0f);
  shouldChangeFontSize = ImGui::IsItemDeactivatedAfterEdit ();
  if (ImGui::Button ("Test Window")) _showDemoWindow ^= true;

  if (_showDemoWindow)
    {
      ImGui::SetNextWindowPos (ImVec2 (650, 20), ImGuiCond_FirstUseEver);
      ImGui::ShowDemoWindow ();
    }
}

ImGuiExample::GuiState
ImGuiExample::createAccountPopup (CreateAccount &createAccountState)
{
  if (WebserviceController::hasLoginState ())
    {
      if (WebserviceController::isLoggedIn ())
        {
          return Lobby{};
        }
    }
  auto const windowWidth = static_cast<float> (windowSize ().x ());
  auto const windowHeight = static_cast<float> (windowSize ().y ());
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
      ImGui::PushFont (font2);
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

ImGuiExample::GuiState
ImGuiExample::login (Login &loginState)
{
  if (WebserviceController::hasLoginState ())
    {
      if (WebserviceController::isLoggedIn ())
        {
          return Lobby{};
        }
    }
  auto const windowWidth = static_cast<float> (windowSize ().x ());
  auto const windowHeight = static_cast<float> (windowSize ().y ());
  ImGui::SetNextWindowSize (ImVec2 (windowWidth, windowHeight));
  ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (5 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
  ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Sign in to XYZ").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
  ImGui::SameLine ();
  ImGui::PushFont (font2);
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
  if (loginState.shouldOpenCreateAnAccount)
    {
      if (WebserviceController::isCreateAccountError ())
        {
          createAccountErrorPopup ();
        }
      else
        {
          createAccountPopup (createAccountData);
          return CreateAccount{};
        }
    }
  return loginState;
}

void
ImGuiExample::updateFontSize ()
{
  ImGuiIO &io = ImGui::GetIO ();
  io.Fonts->Clear ();
  auto currentFont = io.Fonts->AddFontFromFileTTF ("/usr/share/fonts/TTF/SourceSansPro-Regular.ttf", 50 * _fontScale);
  font2 = io.Fonts->AddFontFromFileTTF ("/usr/share/fonts/TTF/SourceSansPro-Regular.ttf", 75 * _fontScale);
  _imgui.relayout ({ static_cast<float> (windowSize ().x ()), static_cast<float> (windowSize ().y ()) }, windowSize (), framebufferSize ());
  ImGui::SetCurrentFont (currentFont);
}

void
ImGuiExample::loginErrorPopup ()
{
  auto const windowWidth = static_cast<float> (windowSize ().x ());
  auto const windowHeight = static_cast<float> (windowSize ().y ());
  ImGui::OpenPopup ("my_select_popup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Login error").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (font2);
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
        }
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
}

void
ImGuiExample::createAccountErrorPopup ()
{
  auto const windowWidth = static_cast<float> (windowSize ().x ());
  auto const windowHeight = static_cast<float> (windowSize ().y ());
  ImGui::OpenPopup ("createAccountErrorPopup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  if (ImGui::BeginPopup ("createAccountErrorPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("Create Account Error").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (font2);
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
        }
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
}

ImGuiExample::GuiState
ImGuiExample::lobby (Lobby &lobbyState)
{
  if (not(WebserviceController::hasLoginState () && WebserviceController::isLoggedIn ()))
    {
      return Login{};
    }
  ImGuiStyle &style = ImGui::GetStyle ();
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

  if (ImGui::Button ("Create Game", ImVec2 (-1, 0)))
    {
      std::cout << "CREATE GAME LOBBY" << std::endl;
    }
  if (ImGui::Button ("Join Game", ImVec2 (-1, 0)))
    {
      std::cout << "CREATE GAME LOBBY" << std::endl;
    }
  if (ImGui::Button ("Logout", ImVec2 (-1, 0)))
    {
      std::cout << "LOGOUT" << std::endl;
      // TODO send logout to server
      WebserviceController::sendObject (shared_class::LogoutAccount{});
    }
  return lobbyState;
}

void
ImGuiExample::drawEvent ()
{
  GL::defaultFramebuffer.clear (GL::FramebufferClear::Color);
  _imgui.newFrame ();

  /* Enable text input, if needed  */
  if (ImGui::GetIO ().WantTextInput && !isTextInputActive ()) startTextInput ();
  else if (!ImGui::GetIO ().WantTextInput && isTextInputActive ())
    stopTextInput ();
  auto const windowWidth = static_cast<float> (windowSize ().x ());
  auto const windowHeight = static_cast<float> (windowSize ().y ());
  ImGui::SetNextWindowSize (ImVec2 (windowWidth, windowHeight));
  ImGui::Begin ("main window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

  // call login() on Login::login() and not ImGuiExample::login()

  auto visit = overloaded{
    [&] (Login &arg) {
      // std::cout << "Login" << std::endl;
      guiState = login (arg);
    },

    [&] (CreateAccount &arg) {
      // std::cout << "CreateAccount" << std::endl;
      guiState = createAccountPopup (arg);
    },

    [&] (Lobby &arg) {
      // std::cout << "Lobby" << std::endl;
      guiState = lobby (arg);
    },
  };
  std::visit (visit, guiState);

  // if (not WebserviceController::hasLoginState ()) login (loginData);
  // else
  //   {
  //     if (WebserviceController::isLoggedIn ())
  //       {
  //         if (false)
  //           {
  //             if (false)
  //               {
  //                 // create game view
  //               }
  //             else
  //               {
  //                 // join game view
  //               }
  //           }
  //         else
  //           {
  //             lobby (lobbyData);
  //           }
  //       }
  //     else
  //       {
  //         loginErrorPopup ();
  //       }
  //   }

  auto shouldUpdateFontSize = false;
  debug (shouldUpdateFontSize);

  ImGui::End ();
  /* Update application cursor */
  _imgui.updateApplicationCursor (*this);
  /* Set appropriate states. If you only draw ImGui, it is sufficient to
     just enable blending and scissor test in the constructor. */
  GL::Renderer::enable (GL::Renderer::Feature::Blending);
  GL::Renderer::enable (GL::Renderer::Feature::ScissorTest);
  GL::Renderer::disable (GL::Renderer::Feature::FaceCulling);
  GL::Renderer::disable (GL::Renderer::Feature::DepthTest);
  _imgui.drawFrame ();
  /* Reset state. Only needed if you want to draw something else with
     different state after. */
  GL::Renderer::enable (GL::Renderer::Feature::DepthTest);
  GL::Renderer::enable (GL::Renderer::Feature::FaceCulling);
  GL::Renderer::disable (GL::Renderer::Feature::ScissorTest);
  GL::Renderer::disable (GL::Renderer::Feature::Blending);
  swapBuffers ();
  redraw ();
  if (shouldUpdateFontSize) updateFontSize ();
}

void
ImGuiExample::viewportEvent (ViewportEvent &event)
{
  GL::defaultFramebuffer.setViewport ({ {}, event.framebufferSize () });

  _imgui.relayout (Vector2{ event.windowSize () } / event.dpiScaling (), event.windowSize (), event.framebufferSize ());
}

void
ImGuiExample::keyPressEvent (KeyEvent &event)
{
  if (_imgui.handleKeyPressEvent (event)) return;
}

void
ImGuiExample::keyReleaseEvent (KeyEvent &event)
{
  if (_imgui.handleKeyReleaseEvent (event)) return;
}

void
ImGuiExample::mousePressEvent (MouseEvent &event)
{
  if (_imgui.handleMousePressEvent (event)) return;
}

void
ImGuiExample::mouseReleaseEvent (MouseEvent &event)
{
  if (_imgui.handleMouseReleaseEvent (event)) return;
}

void
ImGuiExample::mouseMoveEvent (MouseMoveEvent &event)
{
  if (_imgui.handleMouseMoveEvent (event)) return;
}

void
ImGuiExample::mouseScrollEvent (MouseScrollEvent &event)
{
  if (_imgui.handleMouseScrollEvent (event))
    {
      /* Prevent scrolling the page */
      event.setAccepted ();
      return;
    }
}

void
ImGuiExample::textInputEvent (TextInputEvent &event)
{
  if (_imgui.handleTextInputEvent (event)) return;
}
