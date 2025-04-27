# Configuration
BUILD_DIR = build
EXECUTABLE = tank_game
CMAKE = cmake
CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Release

all: all

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

all: $(BUILD_DIR)
	$(CMAKE) -S . -B $(BUILD_DIR) -DUSE_SFML=OFF $(CMAKE_FLAGS)
	$(CMAKE) --build $(BUILD_DIR) --parallel
	cp $(BUILD_DIR)/$(EXECUTABLE) ./$(EXECUTABLE)

# Build and run tests
test: $(BUILD_DIR)
	$(CMAKE) -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)
	$(CMAKE) --build $(BUILD_DIR) --target tank_game_tests --parallel
	cd $(BUILD_DIR) && ctest --verbose

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(EXECUTABLE)
	rm -f output.txt errors.txt input_errors.txt
