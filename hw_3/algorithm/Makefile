ID = 322868852_340849710
TARGET = Algorithm_$(ID).so

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -std=c++20 -fPIC -I../common -I../userCommon

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -shared -o $@ $^

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
