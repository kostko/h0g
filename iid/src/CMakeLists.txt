
add_subdirectory(storage)
add_subdirectory(drivers)
add_subdirectory(scene)
add_subdirectory(renderer)
add_subdirectory(events)
add_subdirectory(entities)
add_subdirectory(gui)

set(iid_src
logger.cpp
context.cpp
gamestate.cpp
timing.cpp
)

add_library(iid STATIC ${iid_src})

# Internal submodules
target_link_libraries(iid storage drivers scene renderer events entities gui)

