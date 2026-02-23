CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
PKG_CONFIG := pkg-config
GTKMM_CFLAGS := $(shell $(PKG_CONFIG) --cflags gtkmm-3.0)
GTKMM_LIBS := $(shell $(PKG_CONFIG) --libs gtkmm-3.0)
.DEFAULT_GOAL := all

SRC := src/main.cpp src/MainWindow.cpp src/RsyncRunner.cpp
OBJ := $(SRC:.cpp=.o)
BIN := quick-backup-gtkmm

.PHONY: all run clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(GTKMM_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(GTKMM_CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
