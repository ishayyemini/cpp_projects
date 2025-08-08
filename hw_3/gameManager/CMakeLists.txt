# Find algorithm sources
file(GLOB GAMEMANAGER_SOURCES "*.cpp")

# Create shared library
add_library(GameManager_322868852_340849710 SHARED ${GAMEMANAGER_SOURCES})

# Include directories
target_include_directories(GameManager_322868852_340849710 PRIVATE
        ../common
        ../userCommon
        ../simulator
)

# Set library properties
set_target_properties(GameManager_322868852_340849710 PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        PREFIX ""  # Remove 'lib' prefix
)
