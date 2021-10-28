#include "src/ui/ui.hxx"
#include "src/controller/commonEvent.hxx"
#include "src/util/imgui_util/imgui_stdlib.h"
#include "src/util/util.hxx"
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Trade/MeshData.h>
#include <algorithm>
#include <boost/math/special_functions/round.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/websocket.h>
#include <filesystem>
#include <game_01_shared_class/serialization.hxx>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <memory>

using namespace Magnum;

ImGuiExample::ImGuiExample (const Arguments &arguments, bool _isTouch, std::string const &websocketAddress) : Magnum::Platform::Application{ arguments, Configuration{}.setTitle ("Magnum ImGui Example").setWindowFlags (Configuration::WindowFlag::Resizable) }, _stateMachine{ StateMachine{ MakeGameMachineData{}, _messagesToSendToServer, logger, MessageBoxPopup{}, std::optional<WaitForServer>{}, textInputString } }, webservice{ _stateMachine }, isTouch (_isTouch)
{
  _imgui = ImGuiIntegration::Context (Vector2{ windowSize () } / dpiScaling (), windowSize (), framebufferSize ());
  co_spawn (
      ioContext, [&] () mutable { return webservice.writeToServer (_messagesToSendToServer.messagesToSendToServer); }, boost::asio::detached);
  EmscriptenWebSocketCreateAttributes ws_attrs = { websocketAddress.c_str (), NULL, EM_TRUE };
  std::cout << "websocketAddress: " << websocketAddress << std::endl;
  EMSCRIPTEN_WEBSOCKET_T ws = emscripten_websocket_new (&ws_attrs);
  emscripten_websocket_set_onopen_callback (ws, &webservice, onopen);
  emscripten_websocket_set_onerror_callback (ws, &webservice, onerror);
  emscripten_websocket_set_onclose_callback (ws, &webservice, onclose);
  emscripten_websocket_set_onmessage_callback (ws, &webservice, onmessage);
  ImGuiIO &io = ImGui::GetIO ();
  std::cout << io.IniFilename << std::endl;
  ImVector<ImWchar> ranges;
  ImFontGlyphRangesBuilder builder;
  builder.AddText (from_u8string (std::u8string{ u8"♠♥♦♣" }).c_str ());
  builder.AddRanges (io.Fonts->GetGlyphRangesDefault ());
  builder.BuildRanges (&ranges);
  auto const fontFile = std::filesystem::path{ "bin/asset/DejaVuSans.ttf" };
  if (std::filesystem::exists (fontFile)) std::cout << "file exists" << std::endl;
  auto const fontSize = 100.0f;
  smallFont = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), fontSize * _fontScale, nullptr, ranges.Data);
  bigFont = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), fontSize * _fontScale * 1.2f, nullptr, ranges.Data);
  io.Fonts->Build ();
  Magnum::GL::Renderer::setBlendEquation (GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
  Magnum::GL::Renderer::setBlendFunction (GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
  ImGuiStyle &style = ImGui::GetStyle ();
  style.Colors[ImGuiCol_Text] = { 1, 1, 1, 1 };
  style.Colors[ImGuiCol_PopupBg] = { 0, 0, 0, 1 };
  style.Colors[ImGuiCol_WindowBg] = { 0, 0, 0, 1 };
}

void
ImGuiExample::debug ()
{
  ImGui::Dummy (ImVec2 (0.0f, static_cast<float> (windowSize ().y ()) / 4));
  ImGui::Text ("Message to Send");
  ImGui::InputText ("##sendMessage", &sendMessage);
  if (ImGui::Button ("Send", ImVec2 (-1, 0)))
    {
      if (not sendMessage.empty ())
        {
          // ignore this debug thing until we need it again
          // WebserviceController::sendMessage (sendMessage);
        }
    }
  ImGui::SliderFloat ("Scale Font", &_fontScale, 0.1f, 1.0f);
  // shouldUpdateFontSize = ImGui::IsItemDeactivatedAfterEdit () ;
  if (ImGui::Button ("Test Window")) _showDemoWindow ^= true;

  if (_showDemoWindow)
    {
      ImGui::SetNextWindowPos (ImVec2 (650, 20), ImGuiCond_FirstUseEver);
      ImGui::ShowDemoWindow ();
    }
}

void
extraCharsButtons (boost::optional<std::string &> &textInputString, float buttonWidth, float buttonHeight, SoftKeyboardState &softKeyboardState)
{
  ImGui::PushStyleVar (ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
  if (ImGui::Button ((softKeyboardState == SoftKeyboardState::specialChars1 || softKeyboardState == SoftKeyboardState::specialChars2) ? "abc" : "?123", ImVec2 (buttonWidth, buttonHeight)))
    {
      softKeyboardState = (softKeyboardState == SoftKeyboardState::specialChars1) ? SoftKeyboardState::smallChars : SoftKeyboardState::specialChars1;
    }
  ImGui::SameLine ();
  if (ImGui::Button (",", ImVec2 (buttonWidth, buttonHeight)))
    {
      textInputString->push_back (',');
    }
  ImGui::SameLine ();
  if (ImGui::Button (" ", ImVec2 (buttonWidth * 6, buttonHeight)))
    {
      textInputString->push_back (' ');
    }
  ImGui::SameLine ();
  if (ImGui::Button (".", ImVec2 (buttonWidth, buttonHeight)))
    {
      textInputString->push_back ('.');
    }
  ImGui::SameLine ();
  if (ImGui::Button ("ENTER", ImVec2 (buttonWidth, buttonHeight)))
    {
      textInputString = {};
    }
  ImGui::PopStyleVar ();
}

void
drawCharsButtons (boost::optional<std::string &> &textInputString, float buttonWidth, float buttonHeight, SoftKeyboardState &softKeyboardState)
{
  ImGui::PushStyleVar (ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
  auto const &chars = (softKeyboardState == SoftKeyboardState::smallChars) ? smallChars : bigChars;
  for (auto i = 0ul; i < chars.size (); i++)
    {
      auto someChar = chars.at (i);
      if (ImGui::Button (std::string{ someChar }.c_str (), ImVec2 (buttonWidth, buttonHeight)))
        {
          textInputString->push_back (someChar);
        }
      if (i != 9 && i != 18 && i != 25)
        {
          ImGui::SameLine ();
        }
      else if (i == 9)
        {
          ImGui::Dummy (ImVec2{ buttonWidth / 2, 0 });
          ImGui::SameLine ();
        }
      else if (i == 18)
        {
          if (ImGui::Button (std::string{ (softKeyboardState == SoftKeyboardState::smallChars) ? "CAPS" : "caps" }.c_str (), ImVec2 (buttonWidth + buttonWidth / 2.f, buttonHeight)))
            {
              softKeyboardState = (softKeyboardState == SoftKeyboardState::smallChars) ? SoftKeyboardState::bigChars : SoftKeyboardState::smallChars;
            }
          ImGui::SameLine ();
        }
      else if (i == 25)
        {
          ImGui::SameLine ();
          if (ImGui::Button (std::string{ "DEL" }.c_str (), ImVec2 (buttonWidth + buttonWidth / 2.f, buttonHeight)))
            {
              if (not textInputString->empty ()) textInputString->pop_back ();
            }
        }
    }
  ImGui::PopStyleVar ();
  extraCharsButtons (textInputString, buttonWidth, buttonHeight, softKeyboardState);
}
void
drawSpecialCharsButtons (boost::optional<std::string &> &textInputString, float buttonWidth, float buttonHeight, SoftKeyboardState &softKeyboardState)
{
  ImGui::PushStyleVar (ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
  auto const &chars = (softKeyboardState == SoftKeyboardState::specialChars1) ? specialChars1 : specialChars2;
  for (auto i = 0ul; i < chars.size (); i++)
    {
      auto someChar = chars.at (i);
      if (ImGui::Button (std::string{ someChar }.c_str (), ImVec2 (buttonWidth, buttonHeight)))
        {
          textInputString->push_back (someChar);
        }
      if (i != 9 && i != 18 && i != 20)
        {
          ImGui::SameLine ();
        }
      else if (i == 9)
        {
          ImGui::Dummy (ImVec2{ buttonWidth / 2, 0 });
          ImGui::SameLine ();
        }
      else if (i == 18)
        {
          if (ImGui::Button (std::string{ (softKeyboardState == SoftKeyboardState::specialChars1) ? "!#$" : "?123" }.c_str (), ImVec2 (buttonWidth + buttonWidth / 2.f, buttonHeight)))
            {
              softKeyboardState = (softKeyboardState == SoftKeyboardState::specialChars1) ? SoftKeyboardState::specialChars2 : SoftKeyboardState::specialChars1;
            }
          ImGui::SameLine ();
        }
      else if (i == 20)
        {
          ImGui::SameLine ();
          ImGui::Dummy (ImVec2{ 5 * buttonWidth, 0 });
          ImGui::SameLine ();
          if (ImGui::Button (std::string{ "DEL" }.c_str (), ImVec2 (buttonWidth + buttonWidth / 2.f, buttonHeight)))
            {
              textInputString->pop_back ();
            }
        }
    }
  ImGui::PopStyleVar ();
  extraCharsButtons (textInputString, buttonWidth, buttonHeight, softKeyboardState);
}

void
ImGuiExample::drawEvent ()
{
  auto const screenWidht = boost::numeric_cast<float> (windowSize ().x ());
  auto const screenHeight = boost::numeric_cast<float> (windowSize ().y ());
  GL::defaultFramebuffer.clear (GL::FramebufferClear::Color);
  _imgui.newFrame ();
  /* Enable text input, if needed  */
  if (ImGui::GetIO ().WantTextInput && !isTextInputActive ()) startTextInput ();
  else if (!ImGui::GetIO ().WantTextInput && isTextInputActive ())
    stopTextInput ();

  auto const isPortraitOrientation = screenWidht < screenHeight;
  ImGui::SetNextWindowPos (ImVec2 (0, 0));
  ImGui::SetNextWindowSize (ImVec2 (screenWidht, textInputString ? screenHeight / (isPortraitOrientation ? (3.f / 2.f) : 2.f) : screenHeight));
  ImGui::PushStyleVar (ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushFont ((screenWidht < 1000) ? bigFont : smallFont); // use big fonts for smaller window width so text is readable on mobile
  ImGui::Begin ("main window", nullptr, isTouch ? (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse) : ImGuiWindowFlags_NoDecoration);
  _stateMachine.process_event (draw{ .windowSizeX = screenWidht, .windowSizeY = screenHeight, .biggerFont = bigFont, .isTouch = isTouch });

  ImGui::End ();
  if (textInputString)
    {
      // TODO navigate in text is missing. It should be possible to insert letters in the middel
      // TODO text intput opens and hides inputs on the lower side. window should scroll so the input is on top of the screen and not hidden by the keyboard
      //-----------------------------------------------------------------------------------------------

      if (isPortraitOrientation)
        {
          auto const softKeyboardHeight = screenHeight / 3.f;
          ImGui::SetNextWindowPos (ImVec2 (0, screenHeight - softKeyboardHeight));
          ImGui::SetNextWindowSize (ImVec2 (screenWidht, screenHeight));
          ImGui::Begin ("New Window", nullptr, ImGuiWindowFlags_NoDecoration);
          float const buttonWidth = (screenWidht - 2.f * ImGui::GetStyle ().WindowPadding.x) / 10.f;
          if (softKeyboardState == SoftKeyboardState::bigChars || softKeyboardState == SoftKeyboardState::smallChars)
            {
              drawCharsButtons (textInputString, buttonWidth, buttonWidth, softKeyboardState);
            }
          else
            {
              drawSpecialCharsButtons (textInputString, buttonWidth, buttonWidth, softKeyboardState);
            }
        }
      else
        {
          ImGui::SetNextWindowPos (ImVec2 (0, screenHeight / 2.f));
          ImGui::SetNextWindowSize (ImVec2 (screenWidht, screenHeight / 2.f));
          ImGui::Begin ("New Window", nullptr, ImGuiWindowFlags_NoDecoration);
          float const buttonWidth = (screenWidht - 2.f * ImGui::GetStyle ().WindowPadding.x) / 10.f;
          float const buttonHeight = (screenHeight / 2.f - 2 * ImGui::GetStyle ().WindowPadding.y - ImGui::GetStyle ().ItemSpacing.y) / 4.f;
          if (softKeyboardState == SoftKeyboardState::bigChars || softKeyboardState == SoftKeyboardState::smallChars)
            {
              drawCharsButtons (textInputString, buttonWidth, buttonHeight, softKeyboardState);
            }
          else
            {
              drawSpecialCharsButtons (textInputString, buttonWidth, buttonHeight, softKeyboardState);
            }
        }

      ImGui::End ();
    }
  ImGui::PopStyleVar ();
  ImGui::PopFont ();
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
  ioContext.poll_one ();
  redraw ();
}

void
ImGuiExample::viewportEvent (ViewportEvent &event)
{
  std::cout << "event.windowSize () " << event.windowSize ().x () << ", " << event.windowSize ().y () << " event.dpiScaling () " << event.dpiScaling ().x () << ", " << event.dpiScaling ().y () << " event.framebufferSize () " << event.framebufferSize ().x () << ", " << event.framebufferSize ().y () << std::endl;
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
  // ImGuiIO &io = ImGui::GetIO ();
  // io.Fonts->Clear ();
  // auto const fontFile = std::filesystem::path{ "bin/asset/DejaVuSans.ttf" };
  // auto const fontSize = (windowSize ().x () < 1000) ? 125.f : 100.f;
  // auto currentFont = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), fontSize * _fontScale);
  // font2 = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), fontSize * _fontScale);
  // _imgui.relayout (Vector2{ windowSize () } / dpiScaling (), windowSize (), framebufferSize ());
  // ImGui::SetCurrentFont (currentFont);
  // shouldUpdateFontSize = false;
}