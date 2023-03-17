add_library(tinyEXR STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyEXR/miniz.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyEXR/miniz.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyEXR/tinyexr.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyEXR/tinyexr.cpp"
)
target_include_directories(tinyEXR PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tinyEXR/")
target_include_directories(tinyEXR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})