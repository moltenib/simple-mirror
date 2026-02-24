# Simple Mirror

GUI wrapper around `rsync -av --info=progress2 --delete` to mirror an origin folder into a destination.

## Qt version (C++)

The C++ app is split by concern:

- `src/main.cpp`: application startup.
- `src/views/MainWindow.hpp` + `src/views/MainWindow.cpp`: Qt UI, validation, dialogs, and progress display.
- `src/views/DirectoryChooserWidget.hpp` + `src/views/DirectoryChooserWidget.cpp`: directory selection UI row.
- `src/controllers/RsyncRunner.hpp` + `src/controllers/RsyncRunner.cpp`: rsync process lifecycle and output parsing.
- `src/utils/AppSetup.hpp` + `src/utils/AppSetup.cpp`: icon and translation initialization.

### Build

Requirements:

- `make`
- A C++17 compiler
- `pkg-config`
- `Qt6 Widgets`
- `lrelease` (Qt Linguist tools)

Build and run:

```bash
make
./simple-mirror
```

Useful targets:

- `make run`: build and launch
- `make clean`: remove objects and binary
- `make translations`: compile each `resources/locales/<lang>/LC_MESSAGES/simple-mirror.ts` to `.qm`
- `make bundle-rsync`: download and bundle MSYS2 `rsync` into `runtime/msys2`
- `make clean-bundle`: remove bundled MSYS2 rsync files and cache

## Translations (Qt)

English is the default UI language. At startup, the app loads a translation based on the system locale if a matching `.qm` file exists in `resources/locales/<lang>/LC_MESSAGES`.

Included translation files:

- German (`de`)
- Spanish (`es`)
- Portuguese (`pt`)
- Italian (`it`)
- Dutch (`nl`)
- French (`fr`)
- Chinese Simplified (`zh_CN`)
- Japanese (`ja`)

## Windows rsync routine (Qt app)

For `simple-mirror`, rsync is discovered in this order
(checked relative to both the executable directory and current working directory):

1. `SIMPLE_MIRROR_RSYNC` environment variable
2. Bundled paths in the working directory:
   - `runtime/msys2/usr/bin/rsync.exe`
   - `runtime/bin/rsync.exe`
   - `msys2/usr/bin/rsync.exe`
   - `bin/rsync.exe`
3. `PATH` (`rsync.exe`, then `rsync`)

On Windows, `make` automatically runs the rsync bundle step unless you disable it:

```bash
make BUNDLE_RSYNC=0
```

On non-Windows systems, you can still generate the bundle explicitly:

```bash
make bundle-rsync
```

On Windows builds, selected folder paths are converted to MSYS/Cygwin style
(for example `C:\Data\Backup\` becomes `/c/Data/Backup/`) before invoking rsync.
