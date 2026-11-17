# Contributing

- Build and run the tests before sending a change: `cmake --build build && ctest --test-dir build`.
- The windows and the interaction are checked by hand: run Slate on a display such as `Xvfb :99` with `DISPLAY=:99 slate --backend x11`, then `DISPLAY=:99 QT_QPA_PLATFORM=xcb chalk`.
- Keep each change limited to one area.
- Report bugs with the version, the steps to reproduce, and the observed output.

Source layout:

- `src/main.cpp`: the entry point, loads the QML module `Sanar.Chalk` and the type `Bar`.
- `src/SlateState.*`: parsing of the `state` reply of the Slate IPC into plain structures, tested in `tests`.
- `src/SlateClient.*`: the connection to the Slate IPC socket, two sockets (requests and the event subscription), reconnection and the properties for QML.
- `src/PanelShell.*`: placing a window as a panel or as a full screen overlay, the X11 dock type with the strut, or the layer shell properties on Wayland. `src/PanelProperties.*` is the description of those properties, shared with the plugin.
- `layershell`: the Qt shell integration plugin `chalk-layer-shell`, `LayerShell` binds `zwlr_layer_shell_v1`, `LayerSurface` is one layer surface. It uses the private API of QtWaylandClient, so it has to be built with the same Qt as the application. The surface is exposed after the first configure with `updateExposure`, without it Qt never renders. `protocols` holds the XML files that the plugin is generated from.
- `src/Calendar.*`: the month grid (`buildMonth`, tested) and the model of the calendar popup, localized names and the first day of the week of the system.
- `src/qml`: the components of the bar, `Bar.qml` is the window, `CalendarPopup.qml` the calendar window and `WorkspaceChip.qml` a workspace. Colors and icons come from the `Theme` and `Icon` of Sanar Style, do not write colors in QML.

Code style: C++20, four spaces, no comments except the SPDX line at the top of each file, names that explain themselves.

Shape, state layer, type and motion tokens, the `StateLayer` and the `PillButton` live in Sanar Style (`Tokens`, `StateLayer`, `PillButton`), not here. Add a new shared token or a generic component there, and keep only the shell specific pieces in `src/qml`.

`src/NotificationStore.*` is the model and the rules of the notifications without D-Bus, so that the tests cover it. `src/NotificationServer.*` is only the D-Bus glue, and `src/qml/NotificationBanner.qml` the banner. Test the daemon on a separate bus, `dbus-run-session -- sh -c '...'`, never on the bus of your session.

`src/QuickSettings.*` talks to the system (`wpctl`, `nmcli`, logind, sysfs) and keeps the parsers of their output static, so that the tests cover them. `PopupWindow.qml` is the base of the popups (position, animation, shadow, the rounding on X11), build a new popup on it. Never click the controls of the quick settings in a test run, they change the real volume and network of the machine.

- `AppCatalog` parses desktop entries and ranks matches in plain C++ (tested in `tests/AppCatalogTest.cpp`); `LauncherPopup` only shows it. Never call `launch` from a test run, it starts a real program.
