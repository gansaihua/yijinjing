file(GLOB ALL_SOURCES *.cpp)
add_library(yijinjing SHARED ${ALL_SOURCES})
target_include_directories(yijinjing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(yijinjing PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

 
file(GLOB ALL_HEADER_FIELS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${ALL_HEADER_FIELS} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/include)
install(TARGETS yijinjing DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
