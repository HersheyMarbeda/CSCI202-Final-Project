cmake_minimum_required(VERSION 3.1)
project(music_player)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Python3 package
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Find the PyBind11 package
find_package(pybind11 REQUIRED)

# Find the SFML package
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

# Add the music_player shared library
add_library(music_player MODULE music_player.cpp music_player_pybind.cpp)

# Include PyBind11 and SFML headers
target_include_directories(music_player PRIVATE ${pybind11_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS})

# Link the Python and SFML libraries
target_link_libraries(music_player PRIVATE ${Python3_LIBRARIES} pybind11::module sfml-audio)

# Set the output name for the shared library
set_target_properties(music_player PROPERTIES PREFIX "" SUFFIX ".so")