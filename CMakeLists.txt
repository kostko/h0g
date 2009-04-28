project(hog)
cmake_minimum_required(VERSION 2.6)

# Uncomment these lines for optimization
#SET(CMAKE_BUILD_TYPE distribution)
#SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
#SET(CMAKE_C_FLAGS_DISTRIBUTION "-O3")

# Infinite Improbability Drive
add_subdirectory(iid)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/iid/include
)

add_executable(hog main.cpp)
target_link_libraries(hog iid)

