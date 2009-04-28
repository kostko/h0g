
SET(Boost_USE_MULTITHREAD ON)
SET(Boost_USE_STATIC_LIBS OFF)

find_package(Boost 1.37.0 COMPONENTS filesystem REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(Eigen2 REQUIRED)
find_library(LIB_BULLET_MATH NAMES bulletmath)
find_library(LIB_BULLET_COL NAMES bulletcollision)
find_library(LIB_BULLET_DYN NAMES bulletdynamics)

include_directories(
	${Boost_INCLUDE_DIRS}
	${SDL_INCLUDE_DIR}
	${SDLIMAGE_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
	${EIGEN2_INCLUDE_DIR}
)

add_subdirectory(storage)
add_subdirectory(drivers)
add_subdirectory(scene)
add_subdirectory(renderer)

set(iid_src
logger.cpp
context.cpp
)

add_library(iid STATIC ${iid_src})

# Internal submodules
target_link_libraries(iid storage drivers scene renderer)

# External libraries
target_link_libraries(iid
	${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${LIB_BULLET_MATH}
	${LIB_BULLET_COL}
	${LIB_BULLET_DYN}
)

