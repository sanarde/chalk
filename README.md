# Sanar Chalk

## About

Panels and bars of SanarDE, written in C++ and QML. The first panel is a top bar with the workspaces of the Slate window manager, the title of the focused window and a clock. It follows the Material You colors of Sanar Style.

## Dependencies

- Qt 6.8 or newer (Core, Gui, Quick, Network)
- CMake 3.21 or newer, Ninja
- A C++20 compiler
- libxcb and its headers
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

The bar is a dock window of the X11 type: it reserves its space on the top edge of the screen. On a Wayland session it needs `QT_QPA_PLATFORM=xcb`, which runs it through Xwayland, and it does not reserve space there yet.

## Roadmap

- Layer shell support, so that the bar reserves its space in a Wayland session without Xwayland.
- A list of open windows and a launcher.
- A system tray, volume, network and battery.
- A bottom panel or a dock.

## License

GPL-3.0-only. See [LICENSE](LICENSE).
