# Sanar Chalk

## About

Panels and bars of SanarDE, written in C++ and QML, with the goal of a complete desktop shell. The first panel is a top bar in the manner of GNOME Shell: the Activities button on the left, the name of the focused application, the date and time in the middle that open a calendar, and the workspaces of the Slate window manager on the right. It follows the Material You colors of Sanar Style and the language of the system.

## Dependencies

- Qt 6.8 or newer (Core, Gui, Quick, Network)
- CMake 3.21 or newer, Ninja
- A C++20 compiler
- libxcb, libwayland-client and their headers, and the private headers of QtWaylandClient (the layer shell plugin uses them)
- [Sanar Style](https://git.nuros.org/sanarde/style), the shared Material You theme and components

At run time:

- [Slate](https://git.nuros.org/sanarde/slate), the window manager of SanarDE. Chalk talks to it through its IPC socket (`$SLATE_SOCKET` or `$XDG_RUNTIME_DIR/slate.sock`) and shows "Slate is not running" until the socket is there.
- `seuratd` of Sanar Seurat is optional, without it Sanar Style uses its own palette.

## Build

```
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=<prefix of Sanar Style>
cmake --build build
ctest --test-dir build
```

The binary is `build/src/chalk`.

## Usage

Start `chalk` after the session has started, for example with `exec-once = chalk` in `slate.conf`. When Sanar Style is not installed in a system prefix, set `QML_IMPORT_PATH=<prefix>/lib/qt6/qml` and `LD_LIBRARY_PATH=<prefix>/lib`.

On Wayland the bar is a surface of the layer shell protocol (`wlr-layer-shell`), anchored to the top edge with an exclusive zone, so that the windows leave room for it, and the calendar is an overlay layer over the whole screen that closes on a click outside of it or on Escape. Chalk brings its own Qt plugin for that, `chalk-layer-shell`, found next to the binary in `plugins` or in the Qt plugin directory after `cmake --install`. Slate supports the protocol. On X11 the bar is a dock window of the X11 type with a strut and the calendar is a popup window. `QT_QPA_PLATFORM` chooses between them, the default follows the session.

`chalk --calendar` opens the calendar at start.

In a nested compositor without `linux-dmabuf`, such as `slate --backend winit`, Qt Quick cannot use OpenGL, run with `QT_QUICK_BACKEND=software`.

## Activities

The Activities button runs the action `overview` of Slate. On Wayland Slate shows its overview of the windows, on X11 there is no overview and the button does nothing.

## Roadmap

- Notifications (the `org.freedesktop.Notifications` service) with a history in the calendar.
- Quick settings: volume, brightness, Wi-Fi, Bluetooth, power, night mode.
- A list of open windows and a launcher.
- A system tray, volume, network and battery.
- A bottom panel or a dock.

## License

GPL-3.0-only. See [LICENSE](LICENSE).
