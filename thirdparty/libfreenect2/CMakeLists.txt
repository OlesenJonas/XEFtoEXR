file(GLOB_RECURSE SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Define the executable
add_library(libfreenect2 ${SOURCES})
target_include_directories(libfreenect2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
