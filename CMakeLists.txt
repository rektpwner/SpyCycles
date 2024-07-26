cmake_minimum_required(VERSION 3.14.0)
project(
  SpyCycles
  LANGUAGES C
  VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Capstone doesn't seem to provide a CMake config file, so we need to use
# pkg-config to find it.
include(FindPkgConfig)
pkg_check_modules(CAPSTONE REQUIRED capstone)
if(!CAPSTONE_FOUND)
  message(FATAL_ERROR "Capstone not found")
endif()

add_compile_options(-Wall -Wextra -pedantic -Werror)

# Create the spycycles executable
add_executable(spycycles spycycles.c)

target_include_directories(spycycles PRIVATE ${CAPSTONE_INCLUDE_DIRS})
target_link_libraries(spycycles PRIVATE ${CAPSTONE_LIBRARIES})

# Create a dummy hello world executable to test the tool
add_executable(hello hello_world.c)

# Install the spycycles executable
include(GNUInstallDirs)
install(TARGETS spycycles DESTINATION ${CMAKE_INSTALL_BINDIR})
