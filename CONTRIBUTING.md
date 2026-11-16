# Contributing

- Build and run the tests before sending a change: `cmake --build build && ctest --test-dir build`.
- The windows and the interaction are checked by hand: run Slate on a display such as `Xvfb :99` with `DISPLAY=:99 slate --backend x11`, then `DISPLAY=:99 QT_QPA_PLATFORM=xcb chalk`.
- Keep each change limited to one area.
- Report bugs with the version, the steps to reproduce, and the observed output.

Source layout:

- `src/main.cpp`: the entry point, loads the QML module `Sanar.Chalk` and the type `Bar`.
- `src/SlateState.*`: parsing of the `state` reply of the Slate IPC into plain structures, tested in `tests`.
- `src/SlateClient.*`: the connection to the Slate IPC socket, two sockets (requests and the event subscription), reconnection and the properties for QML.
- `src/PanelShell.*`: placing a window as a panel, the X11 dock type and the strut now, layer shell later.
- `src/qml`: the components of the bar. Colors and icons come from the `Theme` and `Icon` of Sanar Style, do not write colors in QML.

Code style: C++20, four spaces, no comments except the SPDX line at the top of each file, names that explain themselves.
