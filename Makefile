CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC
PKG_CONFIG := pkg-config
QT_CFLAGS := $(shell $(PKG_CONFIG) --cflags Qt6Widgets)
QT_LIBS := $(shell $(PKG_CONFIG) --libs Qt6Widgets)
.DEFAULT_GOAL := all

SRC := src/main.cpp src/MainWindow.cpp src/RsyncRunner.cpp src/DirectoryChooserWidget.cpp
OBJ := $(SRC:.cpp=.o)
BIN := quick-backup-qt

.PHONY: all run clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(QT_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
