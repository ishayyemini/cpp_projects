# Makefile for GameManager shared library

ID = 322868852_340849710
TARGET = GameManager_$(ID).so

# === Source and object files ===
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

# === Compiler settings ===
CXX = g++
CXXFLAGS = -Wall -std=c++17 -fPIC -I../common -I../UserCommon

# === Targets ===
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -shared -o $@ $^

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
