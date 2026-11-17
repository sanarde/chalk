# Sanar Chalk

> [!NOTE]
> **Repository Mirrors**
>
> - **git.nuros.org** ([sanarde/chalk](https://git.nuros.org/sanarde/chalk)): primary, self-hosted Forgejo instance, accounts restricted to the core team.
> - **GitHub** ([sanarde/chalk](https://github.com/sanarde/chalk)): mirror for external contributors. Issues and Pull Requests opened here are welcome and are reviewed and processed by the core team.

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

`QT_QPA_PLATFORM` chooses between X11 (`xcb`) and Wayland (`wayland`), the default follows the session. On Wayland Chalk uses `wlr-layer-shell` through its own plugin, `chalk-layer-shell`, which is found next to the binary in `plugins` or in the Qt plugin directory after `cmake --install`.

`chalk --calendar` and `chalk --launcher` open the calendar or the launcher at start.

Chalk owns `org.freedesktop.Notifications`. If another daemon holds the name, Chalk prints a warning and does not take it. Check with `notify-send -a Name "Title" "Body"`.

In a nested compositor without `linux-dmabuf`, such as `slate --backend winit`, run with `QT_QUICK_BACKEND=software`.

## Acknowledgments

- [Qt](https://www.qt.io/), the application framework.

## Links

- [CONTRIBUTING](CONTRIBUTING.md)

## License

GPL-3.0-only. See [LICENSE](LICENSE).
