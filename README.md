# Simple Mirror

Qt GUI wrapper around `rsync -av --info=progress2 --delete` to mirror origin to destination

## Code layout

- `src/main.cpp`: app startup
- `src/views/MainWindow.hpp` + `src/views/MainWindow.cpp`: main UI, validation, dialogs, progress
- `src/views/DirectoryChooserWidget.hpp` + `src/views/DirectoryChooserWidget.cpp`: directory chooser row
- `src/controllers/RsyncRunner.hpp` + `src/controllers/RsyncRunner.cpp`: rsync process and output parsing
- `src/utils/AppSetup.hpp` + `src/utils/AppSetup.cpp`: icon and translation setup

## Build

Requirements:

- `make`
- C++17 compiler
- `pkg-config`
- Qt6 Widgets
- `lrelease` (Qt Linguist tools)

Build:

```bash
make
```

## Windows Build Sequence

Use the `MSYS2 MinGW64` shell in the repo root
No `OS=Windows_NT` override is needed

Install dependencies once:

```bash
pacman -S --needed \
  mingw-w64-x86_64-toolchain \
  mingw-w64-x86_64-pkgconf \
  mingw-w64-x86_64-qt6-base \
  mingw-w64-x86_64-qt6-tools \
  mingw-w64-x86_64-qt6-translations \
  make \
  nsis
```

Build:

```bash
make clean
make
```

Create a runnable `dist` bundle with bundled `rsync`:

```bash
make BUNDLE_RSYNC=1 deploy-windows
```

Create an installer:

```bash
make APP_VERSION=1.0.0 installer-windows
```

Outputs:

- `dist/simple-mirror.exe`
- `simple-mirror-setup-1.0.0.exe`

## Icons

- Runtime app icon uses bundled files first:
  - `resources/icons/icon.png`
  - `resources/icons/icon.ico`
- `make deploy-windows` copies `resources/icons` into `dist/resources/icons`
- Installer UI and shortcut icons use `resources/icons/icon.ico` when present
- Windows executable icon embedding is enabled when `resources/icons/icon.ico` exists

Run:

- Linux: `./simple-mirror`
- Windows output: `dist/simple-mirror.exe`

Targets:

- `make run`: build and run
- `make clean`: remove objects, binary, compiled `.qm`
- `make clean-all`: full cleanup (`clean`, runtime bundle, MSYS2 bundle/cache, `dist`, installer `.exe`)
- `make translations`: compile `resources/locales/<lang>/LC_MESSAGES/simple-mirror.ts` to `.qm`
- `make bundle-rsync`: download and bundle MSYS2 `rsync` into `runtime/msys2`
- `make clean-bundle`: remove bundled rsync and cache
- `make bundle-runtime`: Linux-only runtime bundling (shared libs + Qt plugins + `qt.conf` next to executable)
- `make clean-runtime`: remove files created by `bundle-runtime`
- `make deploy-windows`: create `dist/` with `simple-mirror.exe`, MinGW runtime DLLs, Qt runtime via `windeployqt`, locales, and bundled rsync if present
- `make installer-windows`: run `deploy-windows` and build an NSIS installer (`simple-mirror-setup-<version>.exe`)
- `make windows-all`: Windows full pipeline (`clean-all` + bundled rsync + deploy + installer)
- `make clean-windows-deploy`: remove `dist/`

## Translations

Default language is English
At startup, system locale is used if `resources/locales/<lang>/LC_MESSAGES/simple-mirror.qm` exists

Included: `de`, `es`, `pt`, `it`, `nl`, `fr`, `zh_CN`, `ja`

## Windows rsync resolution

`rsync` is resolved in this order (relative to executable directory and current working directory):

1. `SIMPLE_MIRROR_RSYNC`
2. Bundled paths:
   - `runtime/msys2/usr/bin/rsync.exe`
   - `msys2/usr/bin/rsync.exe`

On Windows, Simple Mirror requires an MSYS2-compatible `rsync` runtime (`msys-2.0.dll`)
Random `PATH` rsync binaries are ignored to avoid mixed-runtime failures

Rsync bundling is separate from normal build:

- default: `make`
- explicit bundle: `make bundle-rsync`
- build + bundle: `make BUNDLE_RSYNC=1`

For runnable packaging outside MSYS2 on Windows:

```bash
make deploy-windows
```

On Windows, selected folder paths are converted to MSYS2 POSIX format before rsync call, example: `C:\Data\Backup\` -> `/cygdrive/c/Data/Backup/`
