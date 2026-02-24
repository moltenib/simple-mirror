CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC -Isrc
LDFLAGS :=
PKG_CONFIG := pkg-config
QT_CFLAGS := $(shell $(PKG_CONFIG) --cflags Qt6Widgets)
QT_LIBS := $(shell $(PKG_CONFIG) --libs Qt6Widgets)
.DEFAULT_GOAL := all
ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

SRC := src/main.cpp src/views/MainWindow.cpp src/controllers/RsyncRunner.cpp src/views/DirectoryChooserWidget.cpp src/utils/AppSetup.cpp
OBJ := $(SRC:.cpp=.o)
BIN := $(ROOT_DIR)/simple-mirror
LOCALE_TS_FILES := $(wildcard resources/locales/*/LC_MESSAGES/simple-mirror.ts)
QM_FILES := $(patsubst %/simple-mirror.ts,%/simple-mirror.qm,$(LOCALE_TS_FILES))
LRELEASE := lrelease
MSYS2_BUNDLE_SCRIPT := scripts/bundle-msys2-rsync.sh
MSYS2_RSYNC_EXE := runtime/msys2/usr/bin/rsync.exe
RUNTIME_MANIFEST := $(ROOT_DIR)/.runtime-libs-manifest

BUNDLE_RSYNC ?= 0
ifeq ($(OS),Windows_NT)
BUNDLE_RSYNC := 1
else
LDFLAGS += -Wl,-rpath,'$$ORIGIN'
endif

ALL_TARGETS := $(BIN)
ifeq ($(BUNDLE_RSYNC),1)
ALL_TARGETS += $(MSYS2_RSYNC_EXE)
endif

.PHONY: all run clean bundle-rsync clean-bundle bundle-runtime clean-runtime translations

all: $(ALL_TARGETS) translations

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(QT_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c $< -o $@

translations: $(QM_FILES)

%/simple-mirror.qm: %/simple-mirror.ts
	$(LRELEASE) $< -qm $@

bundle-rsync: $(MSYS2_RSYNC_EXE)

$(MSYS2_RSYNC_EXE): $(MSYS2_BUNDLE_SCRIPT)
	$(MSYS2_BUNDLE_SCRIPT)

bundle-runtime: $(BIN)
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
				dest="$(ROOT_DIR)/$$base"; \
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
		mkdir -p "$(ROOT_DIR)/plugins"; \
		cp -a "$$qt_plugin_dir/platforms" "$(ROOT_DIR)/plugins/"; \
		find "$(ROOT_DIR)/plugins/platforms" -type f -name '*.so*' | while read -r plugin; do \
			copy_deps "$$plugin"; \
		done; \
	fi; \
	sort -u "$(RUNTIME_MANIFEST)" -o "$(RUNTIME_MANIFEST)"; \
	printf '[Paths]\nPlugins = plugins\n' > "$(ROOT_DIR)/qt.conf"; \
	echo "Runtime dependencies copied next to $(BIN)"

run: $(BIN)
	$(BIN)

clean:
	find src -name '*.o' -delete
	rm -f $(BIN) $(QM_FILES)

clean-bundle:
	rm -rf runtime/msys2 .cache/msys2

clean-runtime:
	if [ -f "$(RUNTIME_MANIFEST)" ]; then xargs -r rm -f < "$(RUNTIME_MANIFEST)"; fi
	rm -f "$(RUNTIME_MANIFEST)" qt.conf
	rm -rf plugins
