file(GLOB_RECURSE SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Define the executable
add_executable(main ${SOURCES})

target_link_libraries(main tinyEXR)
target_link_libraries(main ${freenect2_LIBRARIES})
target_include_directories(main PRIVATE ${freenect2_INCLUDE_DIRS})
