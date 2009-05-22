
set(events_src
dispatcher.cpp
event.cpp
keyboard.cpp
mouse.cpp
)

add_library(events STATIC ${events_src})

