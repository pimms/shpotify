cmake_minimum_required(VERSION 2.8)
project(shpotify)

file(GLOB shpotify_sources
	"src/*.h"
	"src/*.cpp"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_executable(shpotify ${shpotify_sources})
target_link_libraries(shpotify formw spotify asound menuw ncursesw jpeg)
