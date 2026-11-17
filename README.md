# Sanar Chalk

## About

Desktop shell of SanarDE.

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

## Notifications

Chalk is a notification daemon (`org.freedesktop.Notifications`, specification 1.2). New notifications appear as a banner under the bar and stay in the list of the calendar. Critical ones do not expire, up to three actions per notification are shown, and the body may use the markup `<b>`, `<i>`, `<u>` and `<a>`. If another daemon already owns the name (GNOME Shell, dunst and so on) Chalk prints a warning and leaves it alone. Try it with `notify-send -a Name "Title" "Body"`.

## Window list

The bar shows one chip per window of the current workspace, in a stable order. The focused one is filled with the secondary container, minimized ones are dimmed. A click focuses a window, restores a minimized one, and minimizes the focused one. The chips share the room up to the clock and shrink when there are many.

## Launcher

The search button in the bar opens a list of installed applications (`.desktop` files from the XDG data directories, the user ones first). Type to filter: names that start with the text come first, then word starts, then substrings, then keywords, ids and comments. Up and Down move, Return starts the application, Escape closes. Entries that are hidden, terminal only or limited by `OnlyShowIn` and `NotShowIn` for the current desktop are skipped, and the localized name and comment are used. Icons come from the icon theme; a letter is shown when there is none. `chalk --launcher` opens it at start.

## Quick settings

The pill with the status icons at the right end of the bar opens the quick settings: tiles for Wi-Fi, Do not disturb, the dark theme and the sound, the volume and brightness sliders and the battery. The volume goes through `wpctl` (PipeWire), Wi-Fi through `nmcli`, the brightness through the `SetBrightness` of logind (no root needed) and the battery is read from `/sys/class/power_supply`. A control that is not available on the machine is hidden.

## Activities

The Activities button runs the action `overview` of Slate. On Wayland Slate shows its overview of the windows, on X11 there is no overview and the button does nothing.

## Roadmap

- Notifications (the `org.freedesktop.Notifications` service) with a history in the calendar.
- Quick settings: volume, brightness, Wi-Fi, Bluetooth, power, night mode.
- A system tray, volume, network and battery.
- A bottom panel or a dock.

## Links

- [CONTRIBUTING](CONTRIBUTING.md)

## License

GPL-3.0-only. See [LICENSE](LICENSE).
