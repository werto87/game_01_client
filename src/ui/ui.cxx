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
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <filesystem>
#include <game_01_shared_class/serialization.hxx>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <memory>

using namespace Magnum;
ImGuiExample::ImGuiExample (const Arguments &arguments) : Magnum::Platform::Application{ arguments, Configuration{}.setTitle ("Magnum ImGui Example").setWindowFlags (Configuration::WindowFlag::Resizable).setSize (Vector2i{ 800, 600 }) }, _stateMachine{ StateMachine{ MakeGameMachineData{}, _messagesToSendToServer, logger, MessageBoxPopup{}, std::optional<WaitForServer>{} } }, webservice{ _stateMachine }
{
  ImGui::CreateContext ();
  co_spawn (
      ioContext, [&] () mutable { return webservice.writeToServer (_messagesToSendToServer.messagesToSendToServer); }, boost::asio::detached);
  EmscriptenWebSocketCreateAttributes ws_attrs = { "ws://localhost:55555", NULL, EM_TRUE };
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
  io.Fonts->AddFontFromFileTTF (fontFile.c_str (), 50.0f * _fontScale, nullptr, ranges.Data);
  io.Fonts->AddFontFromFileTTF (fontFile.c_str (), 75.0f * _fontScale, nullptr, ranges.Data);
  io.Fonts->Build ();
  _imgui = Magnum::ImGuiIntegration::Context (*ImGui::GetCurrentContext (), windowSize ());
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
          // ignore this debug thing until we need it again
          // WebserviceController::sendMessage (sendMessage);
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

  ImGui::SetNextWindowPos (ImVec2 (0, 0));
  ImGui::SetNextWindowSize (ImVec2 (windowWidth, windowHeight));
  ImGui::Begin ("main window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

  _stateMachine.process_event (draw{ .windowSizeX = windowWidth, .windowSizeY = windowHeight, .biggerFont = font2 });
  // auto shouldUpdateFontSize = false;
  // debug (shouldUpdateFontSize);
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
  // if (shouldUpdateFontSize) updateFontSize ();
  ioContext.poll_one ();
  redraw ();
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
  auto const fontFile = std::filesystem::path{ "bin/asset/DejaVuSans.ttf" };
  auto currentFont = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), 50 * _fontScale);
  font2 = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), 75 * _fontScale);
  _imgui.relayout ({ static_cast<float> (windowSize ().x ()), static_cast<float> (windowSize ().y ()) }, windowSize (), framebufferSize ());
  ImGui::SetCurrentFont (currentFont);
}