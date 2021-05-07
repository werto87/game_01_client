#include "src/ui/ui.hxx"
#include "src/controller/database.hxx"
#include "src/controller/stateMachine.hxx"
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

void
login (LoginState &loginState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
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
  ImGui::InputText ("##username", &loginState.accountName);
  ImGui::Text ("Password");
  ImGui::InputText ("##password", &loginState.password, ImGuiInputTextFlags_Password);
  loginState.signInButtonClicked = ImGui::Button ("Sign in", ImVec2 (-1, 0));
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
  loginState.createAccountButtonClicked = ImGui::SmallButton ("create an account");
  ImGui::PopStyleVar ();
  ImGui::EndChild ();
}

void
createAccountPopup (CreateAccountState &createAccountState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
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
      ImGui::InputText ("##account-username", &createAccountState.accountName);
      ImGui::Text ("Password");
      ImGui::InputText ("##account-password", &createAccountState.password, ImGuiInputTextFlags_Password);
      createAccountState.backClicked = ImGui::Button ("Back");
      ImGui::SameLine ();
      createAccountState.createAccountClicked = ImGui::Button ("Create account");
      ImGui::PopItemWidth ();
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
}

void
waitForServerPopup (WaitForServerPopupState &waitForServerPopupState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("my_select_popup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("WaitForServerPopupState").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text ("WaitForServerPopupState");
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
      ImGui::Text ("Wait For Server");
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
      ImGui::SameLine ();
      waitForServerPopupState.cancelButtonClicked = ImGui::Button ("Cancel");
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
}

void
messageBoxPopup (MessageBoxPopupState &messageBoxPopupState, float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto const windowWidth = windowSizeX;
  auto const windowHeight = windowSizeY;
  ImGui::OpenPopup ("my_select_popup");
  ImGui::SetNextWindowSize (ImVec2 (windowHeight, windowHeight));
  if (ImGui::BeginPopup ("my_select_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
      ImGui::Dummy (ImVec2 (0.0f, (windowHeight - (1 * (ImGui::GetFontSize () + ImGui::GetStyle ().ItemSpacing.y * 2))) / 3));
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
      ImGui::PushStyleVar (ImGuiStyleVar_ChildRounding, 5.0f);
      ImGui::Dummy (ImVec2 ((windowWidth - ImGui::CalcTextSize ("WaitForServerPopupState").x - (8 * ImGui::GetStyle ().ItemSpacing.x)) / 2, 0.0f));
      ImGui::SameLine ();
      ImGui::PushFont (&biggerFont);
      ImGui::Text ("WaitForServerPopupState");
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
      ImGui::Text ("Wait For Server");
      ImGui::EndChild ();
      ImGui::EndChild ();
      ImGui::Dummy (ImVec2 (windowWidth / 4, 0.0f));
      for (auto &button : messageBoxPopupState.buttons)
        {
          ImGui::SameLine ();
          button.second = ImGui::Button (button.first.c_str ());
        }
      ImGui::PopStyleVar ();
      ImGui::EndPopup ();
    }
}

void
chat (ChatState &chatState)
{
  ImGui::Text ("Join Channel");
  ImGui::InputText ("##JoinChannel", &chatState.channelToJoin);
  chatState.joinChannelClicked = ImGui::Button ("Join Channel", ImVec2 (-1, 0));
  auto channelNames = chatState.channelNames ();
  if (ImGui::BeginCombo ("##combo 1", chatState.selectChannelComboBoxName ().c_str ()))
    {
      if ((not chatState.selectedChannelName && not channelNames.empty ()) || (chatState.selectedChannelName && not channelNames.empty () && std::ranges::find (channelNames, chatState.selectedChannelName) == channelNames.end ()))
        {
          chatState.selectedChannelName = channelNames.front ();
        }
      for (auto &&channelName : channelNames)
        {
          const bool is_selected = (chatState.selectedChannelName == channelName);
          if (ImGui::Selectable (channelName.data (), is_selected)) chatState.selectedChannelName = channelName;
          if (is_selected) ImGui::SetItemDefaultFocus ();
        }
      ImGui::EndCombo ();
    }
  ImGui::BeginChild ("scrolling", ImVec2 (0, 500), false, ImGuiWindowFlags_HorizontalScrollbar);
  if (chatState.selectedChannelName && std::ranges::find (channelNames, chatState.selectedChannelName) != channelNames.end ())
    {
      for (auto const &text : chatState.channelMessages.at (chatState.selectedChannelName.value ()))
        {
          ImGui::TextUnformatted (text.data (), text.data () + text.size ());
        }
      if (ImGui::GetScrollY () >= ImGui::GetScrollMaxY ()) ImGui::SetScrollHereY (1.0f);
    }
  ImGui::EndChild ();

  ImGui::Text ("Send to Channel");
  ImGui::InputText ("##SendToChannel", &chatState.messageToSendToChannel);
  chatState.sendMessageClicked = ImGui::Button ("Send to Channel", ImVec2 (-1, 0));
}

void
lobby (LobbyState &lobbyState, ImFont &, ChatState &chatState)
{
  chat (chatState);
  // TODO allow joinin a game
  ImGui::Text ("Create Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##CreateGameLobbyName", &lobbyState.createGameLobbyName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##CreateGameLobbyPassword", &lobbyState.createGameLobbyPassword);
  lobbyState.createCreateGameLobbyClicked = ImGui::Button ("Create Game Lobby", ImVec2 (-1, 0));
  ImGui::Text ("Join Game Lobby");
  ImGui::Text ("Game Lobby Name");
  ImGui::InputText ("##JoinGameLobbyName", &lobbyState.joinGameLobbyName);
  ImGui::Text ("Game Lobby Password");
  ImGui::InputText ("##JoinGameLobbyPassword", &lobbyState.joinGameLobbyPassword);
  lobbyState.createJoinGameLobbyClicked = ImGui::Button ("Join Game Lobby", ImVec2 (-1, 0));
  lobbyState.logoutButtonClicked = ImGui::Button ("Logout", ImVec2 (-1, 0));
}

void
lobbyForCreatingAGame (GameLobbyState &gameLobbyState, ChatState &chatState)
{
  chat (chatState);
  ImGui::Text (std::string{ "max user count: " + std::to_string (gameLobbyState.maxUserInGameLobbyToSend) }.c_str ());
  if (gameLobbyState.accountName == gameLobbyState.accountNamesInGameLobby.at (0))
    {
      ImGui::Text ("set max user count: ");
      ImGui::SameLine ();
      ImGui::InputInt ("##MaxUserCount", &gameLobbyState.maxUserInGameLobby);
      gameLobbyState.sendMaxUserCountClicked = ImGui::Button ("set max user count", ImVec2 (-1, 0));
    }
  for (auto &accountName : gameLobbyState.accountNamesInGameLobby)
    {
      ImGui::Text (accountName.c_str ());
    }
  gameLobbyState.sendMaxUserCountClicked = ImGui::Button ("Start Game", ImVec2 (-1, 0));
  gameLobbyState.leaveGameLobby = ImGui::Button ("Leave Game Lobby", ImVec2 (-1, 0));
}

void
visitAndAdvance (std::shared_ptr<Machine> stateMachine, float windowSizeX, float windowSizeY, ImFont &biggerFont)
{
  auto visit = overloaded{
    [&] (LoginState &loginState) {
      login (loginState, windowSizeX, windowSizeY, biggerFont);
      std::cout << "LoginState" << std::endl;
    },
    [&] (MessageBoxPopupState &messageBoxPopupState) {
      //
      std::cout << "MessageBoxPopupState" << std::endl;
      messageBoxPopup (messageBoxPopupState, windowSizeX, windowSizeY, biggerFont);
    },
    [&] (CreateAccountState &createAccountState) {
      //
      std::cout << "CreateAccountState" << std::endl;
      createAccountPopup (createAccountState, windowSizeX, windowSizeY, biggerFont);
    },
    [&] (LobbyState &lobbyState) {
      //
      std::cout << "LobbyState" << std::endl;
      lobby (lobbyState, biggerFont, const_cast<MakeGameLobby &> (stateMachine->state_cast<const MakeGameLobby &> ()).chatState);
    },
    [&] (GameLobbyState &gameLobbyState) {
      //
      std::cout << "GameLobbyState" << std::endl;
      lobbyForCreatingAGame (gameLobbyState, const_cast<MakeGameLobby &> (stateMachine->state_cast<const MakeGameLobby &> ()).chatState);
    },
    [&] (WaitForServerPopupState &waitForServerPopupState) {
      //
      std::cout << "WaitForServerPopupState" << std::endl;
      waitForServerPopup (waitForServerPopupState, windowSizeX, windowSizeY, biggerFont);
    },
  };
  std::visit (visit, stateMachine->state);
  stateMachine->process_event (EvNextState{});
}

using namespace Magnum;
ImGuiExample::ImGuiExample (const Arguments &arguments, std::shared_ptr<Machine> stateMachine) : Magnum::Platform::Application{ arguments, Configuration{}.setTitle ("Magnum ImGui Example").setWindowFlags (Configuration::WindowFlag::Resizable).setSize (Vector2i{ 800, 600 }, Vector2{ 1, 1 }) }, _stateMachine{ stateMachine }
{
  _stateMachine->initiate ();
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
          // TODO ignore this debug thing until we need it again
          //   WebserviceController::sendMessage (sendMessage);
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

void
ImGuiExample::drawEvent ()
{
  GL::defaultFramebuffer.clear (GL::FramebufferClear::Color);
  _imgui.newFrame ();
  auto const windowWidth = static_cast<float> (windowSize ().x ());
  auto const windowHeight = static_cast<float> (windowSize ().y ());

  /* Enable text input, if needed  */
  if (ImGui::GetIO ().WantTextInput && !isTextInputActive ()) startTextInput ();
  else if (!ImGui::GetIO ().WantTextInput && isTextInputActive ())
    stopTextInput ();

  ImGui::SetNextWindowSize (ImVec2 (windowWidth, windowHeight));
  ImGui::Begin ("main window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

  // testStateMachine.execute (windowWidth, windowHeight, *font2);
  // auto shouldUpdateFontSize = false;
  // debug (shouldUpdateFontSize);
  visitAndAdvance (_stateMachine, windowWidth, windowHeight, *font2);
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
  // if (shouldUpdateFontSize) updateFontSize ();
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