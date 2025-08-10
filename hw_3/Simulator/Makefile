ID = 322868852_340849710
TARGET = simulator_$(ID)

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c++20 -fPIC -I../common -I../UserCommon

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) -ldl -rdynamic -pthread

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
