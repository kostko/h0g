project(hog)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/iid/cmake
)

SET(Boost_USE_MULTITHREAD ON)
SET(Boost_USE_STATIC_LIBS OFF)

find_package(Boost 1.37.0 COMPONENTS filesystem signals REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(Eigen2 REQUIRED)
find_package(Bullet REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ALUT REQUIRED)

# Uncomment these lines for optimization
#SET(CMAKE_BUILD_TYPE distribution)
#SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
#SET(CMAKE_C_FLAGS_DISTRIBUTION "-O3")

include_directories(
	# External includes
	${Boost_INCLUDE_DIRS}
	${SDL_INCLUDE_DIR}
	${SDLIMAGE_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
	${EIGEN2_INCLUDE_DIR}
	${BULLET_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${ALUT_INCLUDE_DIR}
	
	# Internal includes
	${CMAKE_CURRENT_SOURCE_DIR}/iid/include
)

# Infinite Improbability Drive
add_subdirectory(iid)

set(hog_src
main.cpp
motionstate.cpp
robot.cpp
weapon.cpp
crate.cpp
enemy.cpp
toad.cpp
ai.cpp
)

add_executable(hog ${hog_src})
target_link_libraries(hog iid)

# External libraries
target_link_libraries(hog
	${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${BULLET_LIBRARIES}
	${OPENAL_LIBRARY}
	${ALUT_LIBRARY}
)

