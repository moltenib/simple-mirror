CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC -Isrc
PKG_CONFIG := pkg-config
QT_CFLAGS := $(shell $(PKG_CONFIG) --cflags Qt6Widgets)
QT_LIBS := $(shell $(PKG_CONFIG) --libs Qt6Widgets)
.DEFAULT_GOAL := all
ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

SRC := src/main.cpp src/views/MainWindow.cpp src/controllers/RsyncRunner.cpp src/views/DirectoryChooserWidget.cpp src/utils/AppSetup.cpp
OBJ := $(SRC:.cpp=.o)
BIN := $(ROOT_DIR)/simple-mirror
TS_DIR := resources/locales/ts
QM_DIR := resources/locales/qm
TS_FILES := $(wildcard $(TS_DIR)/simple-mirror_*.ts)
QM_FILES := $(patsubst $(TS_DIR)/%.ts,$(QM_DIR)/%.qm,$(TS_FILES))
LRELEASE := lrelease
MSYS2_BUNDLE_SCRIPT := scripts/bundle-msys2-rsync.sh
MSYS2_RSYNC_EXE := runtime/msys2/usr/bin/rsync.exe

BUNDLE_RSYNC ?= 0
ifeq ($(OS),Windows_NT)
BUNDLE_RSYNC := 1
endif

ALL_TARGETS := $(BIN)
ifeq ($(BUNDLE_RSYNC),1)
ALL_TARGETS += $(MSYS2_RSYNC_EXE)
endif

.PHONY: all run clean bundle-rsync clean-bundle translations

all: $(ALL_TARGETS) translations

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(QT_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c $< -o $@

translations: $(QM_FILES)

$(QM_DIR):
	mkdir -p $(QM_DIR)

$(QM_DIR)/%.qm: $(TS_DIR)/%.ts | $(QM_DIR)
	$(LRELEASE) $< -qm $@

bundle-rsync: $(MSYS2_RSYNC_EXE)

$(MSYS2_RSYNC_EXE): $(MSYS2_BUNDLE_SCRIPT)
	$(MSYS2_BUNDLE_SCRIPT)

run: $(BIN)
	$(BIN)

clean:
	find src -name '*.o' -delete
	rm -f $(BIN) $(QM_FILES)

clean-bundle:
	rm -rf runtime/msys2 .cache/msys2
