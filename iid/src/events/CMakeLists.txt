
set(events_src
dispatcher.cpp
event.cpp
keyboard.cpp
)

add_library(events STATIC ${events_src})

