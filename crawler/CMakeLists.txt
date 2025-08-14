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

# Config
set(SEARCHLIGHT_CRAWLER_USER_AGENT_NAME "Searchlight Crawler")
set(SEARCHLIGHT_DEFAULT_CRAWL_DELAY 2)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config/config.hpp")

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(searchlight-crawler ${SRC_FILES})

target_include_directories(searchlight-crawler
                           PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/config)
target_link_libraries(searchlight-crawler PRIVATE ${CURL_LIBRARIES} ada::ada)
