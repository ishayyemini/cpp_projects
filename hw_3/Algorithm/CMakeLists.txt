# Find algorithm sources
file(GLOB ALGORITHM_SOURCES "*.cpp")

# Create shared library
add_library(Algorithm_322868852_340849710 SHARED ${ALGORITHM_SOURCES})

# Include directories
target_include_directories(Algorithm_322868852_340849710 PRIVATE
        ../common
        ../UserCommon
)

# Set library properties
set_target_properties(Algorithm_322868852_340849710 PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        PREFIX ""  # Remove 'lib' prefix
)
