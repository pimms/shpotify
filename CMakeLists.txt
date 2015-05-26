cmake_minimum_required(VERSION 2.8)
project(shpotify)

file(GLOB shpotify_sources
	"src/*.h"
	"src/*.c"
)

add_executable(shpotify ${shpotify_sources})

target_link_libraries(shpotify formw spotify asound menuw ncursesw jpeg)
