file(GLOB ALL_SOURCES *.cpp)
add_library(yijinjing SHARED ${ALL_SOURCES})
target_include_directories(yijinjing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(yijinjing PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
