find_package(fmt REQUIRED)
find_package(eventpp CONFIG REQUIRED)

file(GLOB ALL_SOURCES *.cpp)
add_library(yijinjing SHARED ${ALL_SOURCES})
target_include_directories(yijinjing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(yijinjing PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(yijinjing PRIVATE fmt::fmt)
 
file(GLOB ALL_HEADER_FIELS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${ALL_HEADER_FIELS} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/yijinjing)
install(TARGETS yijinjing DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
