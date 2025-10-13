cmake_minimum_required(VERSION 3.25)

project(
  searchlight-server
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(searchlight src/main.cpp)

target_include_directories(searchlight PUBLIC include external)

# Find the standard Threads library (for pthread)
find_package(Threads REQUIRED)

# nlohmann/json
find_package(nlohmann_json REQUIRED)

target_link_libraries(searchlight PUBLIC Threads::Threads)

# --- Copy the static assets into the build directory ---
file(COPY static DESTINATION ${CMAKE_BINARY_DIR}/server/)

# --- Make C++ code aware of the 'static' directory ---
# Do not add "" to the path
target_compile_definitions(
  searchlight PRIVATE STATIC_FILE_PATH=${CMAKE_BINARY_DIR}/server/static)

message(STATUS "Executable will be built as 'searchlight'")
