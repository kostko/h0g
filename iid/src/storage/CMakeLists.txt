
add_definitions(-DTIXML_USE_TICPP -DTIXML_USE_STL)
add_subdirectory(tinyxml)
add_subdirectory(importers)

set(storage_src
storage.cpp
arguments.cpp
mesh.cpp
compositemesh.cpp
texture.cpp
shader.cpp
material.cpp
)

add_library(storage STATIC ${storage_src})
target_link_libraries(storage tinyxml importers)

