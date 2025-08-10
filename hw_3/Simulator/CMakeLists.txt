# Find required sources
file(GLOB SIMULATOR_SOURCES "*.cpp")

# Create executable
add_executable(simulator_322868852_340849710 ${SIMULATOR_SOURCES})

# Include directories
target_include_directories(simulator_322868852_340849710 PRIVATE
        ../common
        ../UserCommon
)

# Link libraries
target_link_libraries(simulator_322868852_340849710 PRIVATE
        ${CMAKE_DL_LIBS}  # For dlopen/dlclose
)

# Export symbols for dynamic loading
set_target_properties(simulator_322868852_340849710 PROPERTIES
        ENABLE_EXPORTS ON
)
