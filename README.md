# Quick Back-Up

GUI wrapper around `rsync -aP --delete` to mirror an origin folder into a destination.

## Python version

Runs the original PyGObject app:

```bash
./quick-backup.py
```

## GTKmm version (C++)

The C++ port is split by concern:

- `src/main.cpp`: application startup and icon selection.
- `src/MainWindow.hpp` + `src/MainWindow.cpp`: UI, validation, dialogs, and progress display.
- `src/RsyncRunner.hpp` + `src/RsyncRunner.cpp`: rsync process lifecycle and output/progress parsing.

### Build

Requirements:

- CMake 3.16+
- A C++17 compiler
- `gtkmm-3.0`

Build and run:

```bash
cmake -S . -B build
cmake --build build
./build/quick-backup-gtkmm
```

### Build without CMake

If `cmake` is not installed, you can build directly with `make`:

```bash
make
./quick-backup-gtkmm
```

Useful targets:

- `make run`: build and launch
- `make clean`: remove objects and binary

## Windows rsync routine (GTKmm app)

For `quick-backup-gtkmm`, rsync is discovered in this order:

1. `QUICK_BACKUP_RSYNC` environment variable
2. Bundled paths in the working directory:
   - `runtime/bin/rsync.exe`
   - `msys2/usr/bin/rsync.exe`
   - `bin/rsync.exe`
3. `PATH` (`rsync.exe`, then `rsync`)

On Windows builds, selected folder paths are converted to MSYS/Cygwin style
(for example `C:\Data\Backup\` becomes `/c/Data/Backup/`) before invoking rsync.
