CXX := g++
WINDRES ?= windres
PKG_CONFIG := pkg-config
LRELEASE := lrelease
WINDEPLOYQT ?= windeployqt6
NSIS ?= makensis
APP_VERSION ?= 1.0.0
BUNDLE_RSYNC ?= 0

CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC -Isrc
LDFLAGS :=
QT_CFLAGS := $(shell $(PKG_CONFIG) --cflags Qt6Widgets)
QT_LIBS := $(shell $(PKG_CONFIG) --libs Qt6Widgets)

.DEFAULT_GOAL := all

ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
UNAME_S := $(shell uname -s 2>/dev/null || echo)
IS_WINDOWS := $(if $(filter Windows_NT,$(OS))$(filter MSYS% MINGW% CYGWIN%,$(UNAME_S)),1,0)
WINDOWS_ENV_MSG := only available in a Windows/MSYS2 environment

SRC := src/main.cpp src/views/MainWindow.cpp src/controllers/RsyncRunner.cpp src/views/DirectoryChooserWidget.cpp src/utils/AppSetup.cpp src/utils/Settings.cpp
OBJ := $(SRC:.cpp=.o)

LOCALE_TS_FILES := $(wildcard resources/locales/*/LC_MESSAGES/simple-mirror.ts)
QM_FILES := $(patsubst %/simple-mirror.ts,%/simple-mirror.qm,$(LOCALE_TS_FILES))

MSYS2_BUNDLE_SCRIPT := scripts/bundle-msys2-rsync.sh
WIN_DLL_COLLECT_SCRIPT := scripts/collect-win-dlls.sh
MSYS2_RSYNC_EXE := runtime/msys2/usr/bin/rsync.exe
RUNTIME_MANIFEST := $(ROOT_DIR)/.runtime-libs-manifest
WIN_DEPLOY_DIR := $(ROOT_DIR)/dist
WIN_ICON_ICO := resources/icons/icon.ico
WIN_ICON_RC := resources/icons/app-icon.rc
WIN_ICON_OBJ := resources/icons/app-icon.o
WIN_MINGW_BIN := $(dir $(shell command -v $(CXX) 2>/dev/null))
NSIS_SCRIPT := scripts/simple-mirror-installer.nsi

ifeq ($(IS_WINDOWS),1)
BIN := $(WIN_DEPLOY_DIR)/simple-mirror.exe
LDFLAGS += -mwindows
ifneq ($(wildcard $(WIN_ICON_ICO)),)
OBJ += $(WIN_ICON_OBJ)
endif
else
BIN := $(ROOT_DIR)/simple-mirror
LDFLAGS += -Wl,-rpath,'$$ORIGIN'
endif

BIN_DIR := $(dir $(BIN))
ALL_TARGETS := $(BIN) $(if $(filter 1,$(BUNDLE_RSYNC)),$(MSYS2_RSYNC_EXE))
DEPLOY_WINDOWS_DEPS := $(BIN) translations $(if $(filter 1,$(BUNDLE_RSYNC)),$(MSYS2_RSYNC_EXE))

.PHONY: all run clean clean-all windows-all bundle-rsync clean-bundle bundle-runtime clean-runtime deploy-windows clean-windows-deploy translations installer-windows

all: $(ALL_TARGETS) translations

$(BIN): $(OBJ)
	@mkdir -p "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(QT_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c $< -o $@

$(WIN_ICON_OBJ): $(WIN_ICON_RC) $(WIN_ICON_ICO)
	$(WINDRES) -i $(WIN_ICON_RC) -o $@

translations: $(QM_FILES)

%/simple-mirror.qm: %/simple-mirror.ts
	$(LRELEASE) $< -qm $@

bundle-rsync: $(MSYS2_RSYNC_EXE)

$(MSYS2_RSYNC_EXE): $(MSYS2_BUNDLE_SCRIPT)
	$(MSYS2_BUNDLE_SCRIPT)

deploy-windows: $(DEPLOY_WINDOWS_DEPS)
ifeq ($(IS_WINDOWS),1)
	@set -eu; \
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
	fi; \
	if command -v "$(WINDEPLOYQT)" >/dev/null 2>&1; then \
		"$(WINDEPLOYQT)" --release --no-translations --no-quick-import "$(BIN)"; \
	elif command -v windeployqt >/dev/null 2>&1; then \
		windeployqt --release --no-translations --no-quick-import "$(BIN)"; \
	else \
		echo "Warning: windeployqt not found, Qt runtime was not auto-copied"; \
	fi; \
	echo "Collecting runtime DLL dependencies with ldd (this may take a while)"; \
	bash "$(WIN_DLL_COLLECT_SCRIPT)" "$(WIN_DEPLOY_DIR)" "$(WIN_MINGW_BIN)" "$$qt_bin_dir"; \
	echo "Windows deployment is ready in $(WIN_DEPLOY_DIR)"
else
	@echo "deploy-windows is $(WINDOWS_ENV_MSG)"
	@exit 1
endif

installer-windows: deploy-windows
ifeq ($(IS_WINDOWS),1)
	@if ! command -v "$(NSIS)" >/dev/null 2>&1; then \
		echo "installer-windows requires NSIS (makensis)"; \
		exit 1; \
	fi
	MSYS2_ARG_CONV_EXCL='/INPUTCHARSET;/OUTPUTCHARSET' \
	"$(NSIS)" /INPUTCHARSET UTF8 /OUTPUTCHARSET UTF8 -DAPP_VERSION="$(APP_VERSION)" "$(NSIS_SCRIPT)"
	@echo "NSIS installer created: $(ROOT_DIR)/simple-mirror-setup-$(APP_VERSION).exe"
else
	@echo "installer-windows is $(WINDOWS_ENV_MSG)"
	@exit 1
endif

windows-all:
ifeq ($(IS_WINDOWS),1)
	$(MAKE) BUNDLE_RSYNC=1 installer-windows
else
	@echo "windows-all is $(WINDOWS_ENV_MSG)"
	@exit 1
endif

bundle-runtime: $(BIN)
ifeq ($(IS_WINDOWS),1)
	@echo "bundle-runtime is Linux-only Use deploy-windows on Windows"
	@exit 1
else
	@set -eu; \
	command -v ldd >/dev/null 2>&1 || { echo "bundle-runtime requires ldd"; exit 1; }; \
	: > "$(RUNTIME_MANIFEST)"; \
	copy_deps() { \
		target="$$1"; \
		ldd "$$target" \
			| awk '/=>/ {print $$3} /^[[:space:]]*\/.*[[:space:]]+\(/ {print $$1}' \
			| sort -u \
			| while read -r lib; do \
				[ -n "$$lib" ] || continue; \
				[ -f "$$lib" ] || continue; \
				base="$$(basename "$$lib")"; \
				case "$$base" in linux-vdso.so*|ld-linux*.so*|ld-musl-*.so*) continue ;; esac; \
				dest="$(BIN_DIR)$$base"; \
				echo "$$dest" >> "$(RUNTIME_MANIFEST)"; \
				if [ ! -e "$$dest" ]; then \
					cp -L "$$lib" "$$dest"; \
				fi; \
			done; \
	}; \
	copy_deps "$(BIN)"; \
	qt_plugin_dir="$$(qtpaths6 --plugin-dir 2>/dev/null || true)"; \
	if [ -z "$$qt_plugin_dir" ]; then qt_plugin_dir="$$(qtpaths --plugin-dir 2>/dev/null || true)"; fi; \
	if [ -z "$$qt_plugin_dir" ]; then qt_plugin_dir="$$(pkg-config --variable=pluginsdir Qt6Core 2>/dev/null || true)"; fi; \
	if [ -n "$$qt_plugin_dir" ] && [ -d "$$qt_plugin_dir/platforms" ]; then \
		mkdir -p "$(BIN_DIR)plugins"; \
		cp -a "$$qt_plugin_dir/platforms" "$(BIN_DIR)plugins/"; \
		find "$(BIN_DIR)plugins/platforms" -type f -name '*.so*' | while read -r plugin; do \
			copy_deps "$$plugin"; \
		done; \
	fi; \
	sort -u "$(RUNTIME_MANIFEST)" -o "$(RUNTIME_MANIFEST)"; \
	printf '[Paths]\nPlugins = plugins\n' > "$(BIN_DIR)qt.conf"; \
	echo "Runtime dependencies copied next to $(BIN)"
endif

run: $(BIN)
	$(BIN)

clean:
	find src -name '*.o' -delete
	rm -f "$(WIN_ICON_OBJ)"
	rm -f $(BIN) $(QM_FILES)

clean-all: clean clean-runtime clean-bundle clean-windows-deploy
	rm -f "$(ROOT_DIR)"/simple-mirror-setup-*.exe
	rm -rf "$(ROOT_DIR)/.cache"

clean-bundle:
	rm -rf runtime/msys2 .cache/msys2

clean-windows-deploy:
	rm -rf "$(WIN_DEPLOY_DIR)"

clean-runtime:
	if [ -f "$(RUNTIME_MANIFEST)" ]; then xargs -r rm -f < "$(RUNTIME_MANIFEST)"; fi
	rm -f "$(RUNTIME_MANIFEST)" "$(BIN_DIR)qt.conf"
	rm -rf "$(BIN_DIR)plugins"
