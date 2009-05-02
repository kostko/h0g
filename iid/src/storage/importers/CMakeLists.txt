
set(importers_src
base.cpp
mesh.cpp
image.cpp
3ds.cpp
glsl.cpp
collada.cpp
)

add_library(importers STATIC ${importers_src})

