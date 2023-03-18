file(GLOB_RECURSE SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Define the executable
# add_executable(main ${SOURCES})
add_executable(XEFtoEXR ${SOURCES})

target_link_libraries(XEFtoEXR tinyEXR)
target_link_libraries(XEFtoEXR libfreenect2)
target_link_libraries(XEFtoEXR json)
