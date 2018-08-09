#include <condition_variable>
#include <gdk/gdkwayland.h>
#include <thread>
#include <fstream>
#include "bar.hpp"
#include "client.hpp"
#include "factory.hpp"
#include "util/chrono.hpp"

waybar::Bar::Bar(Client &client, std::unique_ptr<struct wl_output *> &&p_output)
  : client(client), window{Gtk::WindowType::WINDOW_TOPLEVEL},
    output(std::move(p_output))
{
  static const struct wl_output_listener outputListener = {
    .geometry = _handleGeometry,
    .mode = _handleMode,
    .done = _handleDone,
    .scale = _handleScale,
  };
  wl_output_add_listener(*output, &outputListener, this);
  window.set_title("waybar");
  window.set_decorated(false);
  _setupConfig();
  _setupCss();
  _setupWidgets();
  gtk_widget_realize(GTK_WIDGET(window.gobj()));
  GdkWindow *gdkWindow = gtk_widget_get_window(GTK_WIDGET(window.gobj()));
  gdk_wayland_window_set_use_custom_surface(gdkWindow);
  surface = gdk_wayland_window_get_wl_surface(gdkWindow);
  layerSurface = zwlr_layer_shell_v1_get_layer_surface(
    client.layer_shell, surface, *output, ZWLR_LAYER_SHELL_V1_LAYER_TOP,
    "waybar");
  zwlr_layer_surface_v1_set_anchor(layerSurface,
    ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
    ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
  zwlr_layer_surface_v1_set_size(layerSurface, _width, client.height);
  static const struct zwlr_layer_surface_v1_listener layerSurfaceListener = {
    .configure = _layerSurfaceHandleConfigure,
    .closed = _layerSurfaceHandleClosed,
  };
  zwlr_layer_surface_v1_add_listener(layerSurface, &layerSurfaceListener,
    this);
  wl_surface_commit(surface);
}

void waybar::Bar::_handleGeometry(void *data, struct wl_output *wl_output,
  int32_t x, int32_t y, int32_t physical_width, int32_t physical_height,
  int32_t subpixel, const char *make, const char *model, int32_t transform)
{
  // Nothing here
}

void waybar::Bar::_handleMode(void *data, struct wl_output *wl_output,
  uint32_t f, int32_t w, int32_t h, int32_t refresh)
{
  auto o = reinterpret_cast<waybar::Bar *>(data);
  o->setWidth(w);
}

void waybar::Bar::_handleDone(void *data, struct wl_output *)
{
  // Nothing here
}

void waybar::Bar::_handleScale(void *data, struct wl_output *wl_output,
  int32_t factor)
{
  // Nothing here
}

void waybar::Bar::_layerSurfaceHandleConfigure(
  void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial,
  uint32_t width, uint32_t height)
{
  auto o = reinterpret_cast<waybar::Bar *>(data);
  o->window.show_all();
  zwlr_layer_surface_v1_ack_configure(surface, serial);
  if (o->client.height != height)
  {
    height = o->client.height;
    std::cout << fmt::format("New Height: {}", height) << std::endl;
    zwlr_layer_surface_v1_set_size(surface, width, height);
    zwlr_layer_surface_v1_set_exclusive_zone(surface, o->visible ? height : 0);
    wl_surface_commit(o->surface);
  }
}

void waybar::Bar::_layerSurfaceHandleClosed(void *data,
  struct zwlr_layer_surface_v1 *surface)
{
  auto o = reinterpret_cast<waybar::Bar *>(data);
  zwlr_layer_surface_v1_destroy(o->layerSurface);
  o->layerSurface = nullptr;
  wl_surface_destroy(o->surface);
  o->surface = nullptr;
  o->window.close();
}

auto waybar::Bar::setWidth(int width) -> void
{
  if (width == this->_width) return;
  std::cout << fmt::format("Bar width configured: {}", width) << std::endl;
  this->_width = width;
  window.set_size_request(width);
  window.resize(width, client.height);
  zwlr_layer_surface_v1_set_size(layerSurface, width, 40);
  wl_surface_commit(surface);
}

auto waybar::Bar::toggle() -> void
{
  visible = !visible;
  auto zone = visible ? client.height : 0;
  zwlr_layer_surface_v1_set_exclusive_zone(layerSurface, zone);
  wl_surface_commit(surface);
}

auto waybar::Bar::_setupConfig() -> void
{
  Json::Value root;
  Json::CharReaderBuilder builder;
  Json::CharReader* reader = builder.newCharReader();
  std::string err;
  std::ifstream file(client.configFile);
  if (!file.is_open())
    throw std::runtime_error("Can't open config file");
  std::string str((std::istreambuf_iterator<char>(file)),
    std::istreambuf_iterator<char>());
  bool res = reader->parse(str.c_str(), str.c_str() + str.size(), &_config, &err);
  delete reader;
  if (!res)
    throw std::runtime_error(err);
}

auto waybar::Bar::_setupCss() -> void
{
  _cssProvider = Gtk::CssProvider::create();
  _styleContext = Gtk::StyleContext::create();

  // load our css file, wherever that may be hiding
  if (_cssProvider->load_from_path(client.cssFile)) {
    Glib::RefPtr<Gdk::Screen> screen = window.get_screen();
    _styleContext->add_provider_for_screen(screen, _cssProvider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  }
}

auto waybar::Bar::_setupWidgets() -> void
{
  auto &left = *Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));
  auto &center = *Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));
  auto &right = *Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));

  auto &box1 = *Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));
  window.add(box1);
  box1.set_homogeneous(true);
  box1.pack_start(left, true, true);
  box1.pack_start(center, false, false);
  box1.pack_end(right, true, true);

  Factory factory(*this, _config);

  if (_config["modules-left"]) {
    for (auto name : _config["modules-left"]) {
      auto &module = factory.makeModule(name.asString());
      left.pack_start(module, false, true, 0);
    }
  }
  if (_config["modules-center"]) {
    for (auto name : _config["modules-center"]) {
      auto &module = factory.makeModule(name.asString());
      center.pack_start(module, true, false, 10);
    }
  }
  if (_config["modules-right"]) {
    std::reverse(_config["modules-right"].begin(), _config["modules-right"].end());
    for (auto name : _config["modules-right"]) {
      auto &module = factory.makeModule(name.asString());
      right.pack_end(module, false, false, 0);
    }
  }
}