
add_definitions(-DGL_GLEXT_PROTOTYPES)

set(drivers_src
base.cpp
opengl.cpp
openal.cpp
)

add_library(drivers STATIC ${drivers_src})

