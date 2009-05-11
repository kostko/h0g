
set(importers_src
base.cpp
mesh.cpp
image.cpp
3ds.cpp
glsl.cpp
collada.cpp
audio.cpp
)

add_library(importers STATIC ${importers_src})

