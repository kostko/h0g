project(iid)
cmake_minimum_required(VERSION 2.6)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

add_subdirectory(src)

