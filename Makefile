CXX := g++
WINDRES ?= windres
PKG_CONFIG := pkg-config
LRELEASE := lrelease
WINDEPLOYQT ?= windeployqt6
NSIS ?= makensis

CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC -Isrc
LDFLAGS :=
QT_CFLAGS := $(shell $(PKG_CONFIG) --cflags Qt6Widgets)
QT_LIBS := $(shell $(PKG_CONFIG) --libs Qt6Widgets)

.DEFAULT_GOAL := all

ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
# Version used for installer filename and Windows "DisplayVersion"
# Override via environment: `APP_VERSION=1.2.3 make windows-installer`
APP_VERSION ?= $(shell cd "$(ROOT_DIR)" && (git describe --tags --dirty --always 2>/dev/null || true) | sed 's/^v//' | tr -d '\r\n')
APP_VERSION := $(strip $(APP_VERSION))
INSTALLER_BASENAME := simple-mirror-setup
ifeq ($(APP_VERSION),)
INSTALLER_FILENAME := $(INSTALLER_BASENAME).exe
else
INSTALLER_FILENAME := $(INSTALLER_BASENAME)-$(APP_VERSION).exe
endif
UNAME_S := $(shell uname -s 2>/dev/null || echo)
IS_WINDOWS := $(if $(filter Windows_NT,$(OS))$(filter MSYS% MINGW% CYGWIN%,$(UNAME_S)),1,0)
WINDOWS_ENV_MSG := only available in a Windows/MSYS2 environment

SRC := src/main.cpp src/views/MainWindow.cpp src/views/MainWindowMisc.cpp src/views/ConfirmationDialog.cpp src/views/WelcomeDialog.cpp src/controllers/RsyncRunner.cpp src/views/DirectoryChooserWidget.cpp src/utils/AppSetup.cpp src/utils/Settings.cpp src/utils/DurationFormat.cpp
OBJ := $(SRC:.cpp=.o)

LOCALE_TS_FILES := $(wildcard resources/locales/*/LC_MESSAGES/simple-mirror.ts)
QM_FILES := $(patsubst %/simple-mirror.ts,%/simple-mirror.qm,$(LOCALE_TS_FILES))

MSYS2_BUNDLE_SCRIPT := scripts/bundle-msys2-rsync.sh
WIN_DLL_COLLECT_SCRIPT := scripts/collect-win-dlls.sh
MSYS2_RSYNC_EXE := runtime/msys2/usr/bin/rsync.exe
WIN_DEPLOY_DIR := $(ROOT_DIR)/dist
WIN_ICON_ICO := resources/icons/icon.ico
WIN_ICON_RC := resources/icons/app-icon.rc
WIN_ICON_OBJ := resources/icons/app-icon.o
WIN_MINGW_BIN := $(dir $(shell command -v $(CXX) 2>/dev/null))
NSIS_SCRIPT := scripts/simple-mirror-installer.nsi
WIN_DLLS_LOCK := $(WIN_DEPLOY_DIR)/.collect-win-dlls.lock
WIN_DEPLOY_LOCK := $(WIN_DEPLOY_DIR)/.windows-deploy.lock

ifeq ($(IS_WINDOWS),1)
BIN := $(WIN_DEPLOY_DIR)/simple-mirror.exe
LDFLAGS += -mwindows
ifneq ($(wildcard $(WIN_ICON_ICO)),)
WIN_ICON_LINK_OBJ := $(WIN_ICON_OBJ)
endif
else
BIN := $(ROOT_DIR)/simple-mirror
LDFLAGS += -Wl,-rpath,'$$ORIGIN'
endif

BIN_DIR := $(dir $(BIN))
ALL_TARGETS := $(BIN)
WINDOWS_DEPLOY_DEPS := $(BIN) translations

.PHONY: all run clean clean-all windows-all bundle-rsync clean-bundle windows-deploy windows-clean-deploy translations windows-installer

all: $(ALL_TARGETS) translations

$(BIN): $(OBJ)
	@mkdir -p "$(dir $@)"
ifeq ($(IS_WINDOWS),1)
ifneq ($(wildcard $(WIN_ICON_ICO)),)
	$(WINDRES) -i $(WIN_ICON_RC) -o $(WIN_ICON_OBJ)
endif
endif
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(WIN_ICON_LINK_OBJ) $(QT_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c $< -o $@

translations: $(QM_FILES)

%/simple-mirror.qm: %/simple-mirror.ts
	$(LRELEASE) $< -qm $@

bundle-rsync: $(MSYS2_RSYNC_EXE)

$(MSYS2_RSYNC_EXE): $(MSYS2_BUNDLE_SCRIPT)
	$(MSYS2_BUNDLE_SCRIPT)

windows-deploy: $(WINDOWS_DEPLOY_DEPS)
ifeq ($(IS_WINDOWS),1)
	@set -eu; \
	if [ -f "$(WIN_DEPLOY_LOCK)" ]; then \
		echo "Skipping windows-deploy (lock found: $(WIN_DEPLOY_LOCK))"; \
		exit 0; \
	fi; \
	exe_name="$$(basename "$(BIN)")"; \
	qt_bin_dir="$$(qtpaths6 --query QT_INSTALL_BINS 2>/dev/null || true)"; \
	if [ -z "$$qt_bin_dir" ]; then qt_bin_dir="$$(qtpaths --query QT_INSTALL_BINS 2>/dev/null || true)"; fi; \
	mkdir -p "$(WIN_DEPLOY_DIR)"; \
	if [ "$(BIN)" != "$(ROOT_DIR)/$$exe_name" ] && [ -f "$(ROOT_DIR)/$$exe_name" ]; then \
		rm -f "$(ROOT_DIR)/$$exe_name"; \
	fi; \
	mkdir -p "$(WIN_DEPLOY_DIR)/resources"; \
	cp -a resources/locales resources/icons "$(WIN_DEPLOY_DIR)/resources/"; \
	if [ -f "$(MSYS2_RSYNC_EXE)" ]; then \
		mkdir -p "$(WIN_DEPLOY_DIR)/runtime/msys2/usr/bin"; \
		cp -f "$(MSYS2_RSYNC_EXE)" "$(WIN_DEPLOY_DIR)/runtime/msys2/usr/bin/"; \
		for dll in "$$(dirname "$(MSYS2_RSYNC_EXE)")"/*.dll; do \
			[ -f "$$dll" ] || continue; \
			cp -f "$$dll" "$(WIN_DEPLOY_DIR)/runtime/msys2/usr/bin/"; \
		done; \
	fi; \
	if command -v "$(WINDEPLOYQT)" >/dev/null 2>&1; then \
		"$(WINDEPLOYQT)" --release --no-translations --no-quick-import "$(BIN)"; \
	elif command -v windeployqt >/dev/null 2>&1; then \
		windeployqt --release --no-translations --no-quick-import "$(BIN)"; \
	else \
		echo "Warning: windeployqt not found, Qt runtime was not auto-copied"; \
	fi; \
	if [ ! -f "$(WIN_DLLS_LOCK)" ]; then \
		echo "Collecting runtime DLL dependencies with ldd (this may take a while)"; \
		bash "$(WIN_DLL_COLLECT_SCRIPT)" "$(WIN_DEPLOY_DIR)" "$(WIN_MINGW_BIN)" "$$qt_bin_dir"; \
		touch "$(WIN_DLLS_LOCK)"; \
	else \
		echo "Skipping collect-win-dlls (lock found: $(WIN_DLLS_LOCK))"; \
	fi; \
	touch "$(WIN_DEPLOY_LOCK)"; \
	echo "Windows deployment is ready in $(WIN_DEPLOY_DIR)"
else
	@echo "windows-deploy is $(WINDOWS_ENV_MSG)"
	@exit 1
endif

windows-installer: windows-deploy
ifeq ($(IS_WINDOWS),1)
	@if ! command -v "$(NSIS)" >/dev/null 2>&1; then \
		echo "windows-installer requires NSIS (makensis)"; \
		exit 1; \
	fi
	MSYS2_ARG_CONV_EXCL='/INPUTCHARSET;/OUTPUTCHARSET' \
	"$(NSIS)" /INPUTCHARSET UTF8 /OUTPUTCHARSET UTF8 -DAPP_VERSION="$(APP_VERSION)" -DOUTPUT_NAME="$(INSTALLER_FILENAME)" "$(NSIS_SCRIPT)"
	@echo "NSIS installer created: $(ROOT_DIR)/$(INSTALLER_FILENAME)"
else
	@echo "windows-installer is $(WINDOWS_ENV_MSG)"
	@exit 1
endif

windows-all:
ifeq ($(IS_WINDOWS),1)
	@if [ ! -f "$(WIN_DEPLOY_LOCK)" ]; then \
		$(MAKE) bundle-rsync; \
	fi
	$(MAKE) all
	$(MAKE) windows-installer
else
	@echo "windows-all is $(WINDOWS_ENV_MSG)"
	@exit 1
endif

run: $(BIN)
	$(BIN)

clean:
	find src -name '*.o' -delete
	rm -f "$(WIN_ICON_OBJ)"
	rm -f $(BIN) $(QM_FILES)

clean-all: clean clean-bundle windows-clean-deploy
	rm -f "$(ROOT_DIR)"/simple-mirror-setup*.exe
	rm -rf "$(ROOT_DIR)/.cache"

clean-bundle:
	rm -rf runtime .cache

windows-clean-deploy:
	rm -rf "$(WIN_DEPLOY_DIR)"
