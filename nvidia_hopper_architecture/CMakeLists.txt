cmake_minimum_required(VERSION 3.16)
project(NVIDIAHopperArchitecture VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

# Create executable
add_executable(hopper_architecture ${SOURCES} ${HEADERS})

# Optional: Create a library
add_library(hopper_lib STATIC ${SOURCES})
target_include_directories(hopper_lib PUBLIC include)

# Enable testing
enable_testing()

# Test executable (if tests are added later)
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(hopper_tests ${TEST_SOURCES})
    target_link_libraries(hopper_tests hopper_lib)
    add_test(NAME HopperTests COMMAND hopper_tests)
endif()

# Installation
install(TARGETS hopper_architecture DESTINATION bin)
install(TARGETS hopper_lib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)