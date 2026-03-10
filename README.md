# Simple Mirror

## Introduction

This is a graphical back-up utility, intended to simplify usage of `rsync` on both Windows and Linux. The following parameters are used:

- Origin: the directory to be mirrored. It will not be altered by this program.
- Destination: a directory to copy the differences into. Choosing the correct path is crucial; entering this parameter without care may have unintended consequences.

As with `rsync`, the process is as follows:

1) The two directories are compared.
2) Files that do not exist in the origin are **removed** to keep the destination up to date.
3) Files in the destination with a different modification date from those in the origin folder are copied or overwritten.

Of these, step **2** can be skipped by holding _Shift_ and clicking "Combine"; rather than create a mirror of the origin, this will merge the contents of the two directories into the destination.

## Installation

### Windows

A Windows installer based on the current state of the main branch can be downloaded from the `windows-output` artifact uploaded to the latest pipeline run ([link](https://github.com/moltenib/simple-mirror/actions/workflows/windows.yml)).

### Arch Linux and distributions with `pacman`

On Arch Linux and derivatives, the latest Git version can be downloaded from the Arch User Repository ([link](https://aur.archlinux.org/packages/simple-mirror-git)).

```shell
yay -S simple-mirror-git
```

## Compilation

To build the program locally, the dependencies below must be met:

- `make`;
- A C++17 compiler like `g++`;
- `pkg-config`;
- Qt6 Widgets;
- `lrelease` (Qt Linguist tools).

### Unix-like systems (Linux, SteamOS)

After cloning this repository, the build process on Linux only needs `make`. Ensure to have all dependencies.

### Windows build

Compiling this program on Windows requires an MSYS2 MinGW64 environment.

###### Installing the dependencies

```shell
pacman -S --needed \
  mingw-w64-x86_64-toolchain \
  mingw-w64-x86_64-pkgconf \
  mingw-w64-x86_64-qt6-base \
  mingw-w64-x86_64-qt6-tools \
  mingw-w64-x86_64-qt6-translations \
  make \
  nsis
```

###### Create a simple executable in `dist/`

```bash
make clean
make
```

###### Add DLL dependencies, including `rsync`

```bash
make bundle-rsync
make windows-deploy
```

###### Create an installer

```bash
make windows-installer
```

###### Outputs

- `dist/simple-mirror.exe`
- `simple-mirror-setup-<version>.exe` (from `APP_VERSION`, default derived from `git describe`)

#### To run this program

###### Linux

```shell
./simple-mirror
```

###### Windows

```shell
explorer dist
```

Double-click the output file: `simple-mirror.exe`.

## Technical information

### All `make` targets

- `make run`: build and run.
- `make clean`: remove objects, binary, compiled `.qm`.
- `make clean-all`: full cleanup (`clean`, MSYS2 bundle/cache, `dist`, installer `.exe`).
- `make translations`: compile `resources/locales/<lang>/LC_MESSAGES/simple-mirror.ts` to `.qm`.
- `make bundle-rsync`: download and bundle MSYS2 `rsync` into `runtime/msys2`.
- `make clean-bundle`: remove bundled rsync and cache.
- `make windows-deploy`: create `dist/` with `simple-mirror.exe`, MinGW runtime DLLs, Qt runtime via `windeployqt`, locales, and bundled rsync if present.
- `make windows-installer`: run `windows-deploy` and build an NSIS installer (`simple-mirror-setup-<version>.exe`).
- `make windows-all`: Windows pipeline (`all` + `windows-installer`, and auto `bundle-rsync` when no deploy lock is present).
- `make windows-clean-deploy`: remove `dist/`.

### Translations

The default language is English. On start, the system locale is used if an associated `resources/locales/<lang>/LC_MESSAGES/simple-mirror.qm` exists.

Included: `de`, `es`, `pt`, `it`, `nl`, `fr`, `zh_CN`, `ja`.

### Windows rsync resolution

`rsync` is resolved in this order (relative to executable directory and current working directory):

1. `SIMPLE_MIRROR_RSYNC`
2. Bundled paths:
   - `runtime/msys2/usr/bin/rsync.exe`
   - `msys2/usr/bin/rsync.exe`

### MSYS2 runtime

On Windows, Simple Mirror requires an MSYS2-compatible `rsync` runtime (`msys-2.0.dll`). Random `PATH` rsync binaries are ignored to avoid mixed-runtime error.
