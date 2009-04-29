
set(scene_src
scene.cpp
node.cpp
rendrable.cpp
light.cpp
octree.cpp
)

add_library(scene STATIC ${scene_src})

