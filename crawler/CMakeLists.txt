cmake_minimum_required(VERSION 3.25)

project(
  searchlight-crawler
  VERSION 1.0
  DESCRIPTION "A web crawler for the Searchlight project"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(ada REQUIRED)
find_package(SQLite3 REQUIRED)

# Config
set(SEARCHLIGHT_CRAWLER_USER_AGENT_NAME
    "Searchlight Crawler"
    CACHE STRING "User-Agent name for the Searchlight crawler")
set(SEARCHLIGHT_DEFAULT_CRAWL_DELAY
    2
    CACHE STRING "Default crawl delay in seconds")
set(SEARCHLIGHT_DB_PATH
    "/var/lib/searchlight/searchlight.db"
    CACHE STRING "Path to the Searchlight database")
set(SEARCHLIGHT_FTS_HTML_EXT_PATH
    "/usr/local/lib/fts5html.so"
    CACHE STRING "Path to the FTS5 HTML extension")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config/config.hpp")

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
                           PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/config)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES} ada::ada
                                              SQLite::SQLite3)
