
set(scene_src
scene.cpp
node.cpp
rendrable.cpp
light.cpp
octree.cpp
camera.cpp
particles.cpp
lightmanager.cpp
geometrymeta.cpp
)

add_library(scene STATIC ${scene_src})

