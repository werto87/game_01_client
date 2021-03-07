#include "src/webservice/webservice.hxx"
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData.h>
#include <boost/bind/bind.hpp>
#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>

#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

namespace Magnum
{
namespace Examples
{

using namespace Math::Literals;

class ImGuiExample : public Platform::Application
{
public:
  explicit ImGuiExample (const Arguments &arguments);

  void drawEvent () override;

  void viewportEvent (ViewportEvent &event) override;

  void keyPressEvent (KeyEvent &event) override;
  void keyReleaseEvent (KeyEvent &event) override;

  void mousePressEvent (MouseEvent &event) override;
  void mouseReleaseEvent (MouseEvent &event) override;
  void mouseMoveEvent (MouseMoveEvent &event) override;
  void mouseScrollEvent (MouseScrollEvent &event) override;
  void textInputEvent (TextInputEvent &event) override;

  ImGuiIntegration::Context _imgui{ NoCreate };

private:
  bool _showDemoWindow = true;
  bool _showAnotherWindow = false;
  Color4 _clearColor = 0x72909aff_rgbaf;
  Float _floatValue = 0.5f;
};

ImGuiExample::ImGuiExample (const Arguments &arguments) : Platform::Application{ arguments, Configuration{}.setTitle ("Magnum ImGui Example").setWindowFlags (Configuration::WindowFlag::Resizable) }
{
  ImGui::CreateContext ();
  ImGui::GetIO ().Fonts->AddFontFromFileTTF ("/usr/share/fonts/TTF/SourceSansPro-Regular.ttf", 50.0f * _floatValue);
  _imgui = ImGuiIntegration::Context (*ImGui::GetCurrentContext (), windowSize ());
  /* Set up proper blending to be used by ImGui. There's a great chance
     you'll need this exact behavior for the rest of your scene. If not, set
     this only for the drawFrame() call. */
  GL::Renderer::setBlendEquation (GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
  GL::Renderer::setBlendFunction (GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

#if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
  /* Have some sane speed, please */
  setMinimalLoopPeriod (16);
#endif
}

void
ImGuiExample::drawEvent ()
{

  GL::defaultFramebuffer.clear (GL::FramebufferClear::Color);

  _imgui.newFrame ();

  /* Enable text input, if needed */
  if (ImGui::GetIO ().WantTextInput && !isTextInputActive ()) startTextInput ();
  else if (!ImGui::GetIO ().WantTextInput && isTextInputActive ())
    stopTextInput ();

  ImGui::Begin ("Another Window", &_showAnotherWindow);
  ImGui::SliderFloat ("Scale Font", &_floatValue, 0.1f, 1.0f);
  auto shouldChangeFontSize = ImGui::IsItemDeactivatedAfterEdit ();
  // if (ImGui::ColorEdit3 ("Clear Color", _clearColor.data ())) GL::Renderer::setClearColor (_clearColor);
  if (ImGui::Button ("Test Window")) _showDemoWindow ^= true;
  ImGui::End ();

  if (_showDemoWindow)
    {
      ImGui::SetNextWindowPos (ImVec2 (650, 20), ImGuiCond_FirstUseEver);
      ImGui::ShowDemoWindow ();
    }

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
  if (shouldChangeFontSize)
    {
      ImGui::GetIO ().Fonts->Clear ();
      ImGui::GetIO ().Fonts->AddFontFromFileTTF ("/usr/share/fonts/TTF/SourceSansPro-Regular.ttf", 50.0f * _floatValue);
      _imgui.relayout ({ static_cast<float> (windowSize ().x ()), static_cast<float> (windowSize ().y ()) }, windowSize (), framebufferSize ());
    }
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

}
}

boost::asio::awaitable<void>
read (boost::asio::io_context &io_context, Webservice &webservice)
{
  co_await webservice.connect ();
  boost::asio::co_spawn (
      io_context, [&] () mutable { return webservice.read (); }, boost::asio::detached);
}

boost::asio::awaitable<void>
ui (boost::asio::io_context &io_context, Magnum::Examples::ImGuiExample &app)
{
  for (;;)
    {
      app.mainLoopIteration ();
      auto timer = boost::asio::steady_timer (co_await boost::asio::this_coro::executor);
      using namespace std::chrono_literals;
      timer.expires_after (1ms);
      co_await timer.async_wait (boost::asio::use_awaitable); // give other tasks a chance to run
    }
}

int
main (int argc, char **argv)
{
  try
    {
      Magnum::Examples::ImGuiExample app ({ argc, argv });
      boost::asio::io_context io_context (1);
      boost::asio::signal_set signals (io_context, SIGINT, SIGTERM);
      signals.async_wait ([&] (auto, auto) { io_context.stop (); });
      auto webservice = Webservice{ io_context };
      boost::asio::co_spawn (io_context, boost::bind (read, std::ref (io_context), std::ref (webservice)), boost::asio::detached);
      boost::asio::co_spawn (io_context, boost::bind (ui, std::ref (io_context), std::ref (app)), boost::asio::detached);
      io_context.run ();
    }
  catch (std::exception &e)
    {
      std::printf ("Exception: %s\n", e.what ());
    }
}