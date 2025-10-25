cmake_minimum_required(VERSION 3.18)
project(ASCII_ART)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

set(DIR ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR}/bin/)

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)

include_directories(
  ${DIR}
  ${DIR}/lib
  ${DIR}/lib/inc
  ${DIR}/lin/inc/image
)

file(GLOB image_srcs ${DIR}/lib/src/image/*.c)

add_library(image STATIC ${image_srcs})
target_link_libraries(image m)

add_executable(ascii-art src/ascii-art.c)
target_link_libraries(ascii-art
  image
  m
  ZLIB::ZLIB
  JPEG::JPEG
)

