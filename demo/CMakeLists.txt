link_libraries(yijinjing)

add_executable(demo_writer demo_writer.cpp)
add_executable(demo_writer2 demo_writer2.cpp)
add_executable(demo_reader demo_reader.cpp)

find_package(eventpp CONFIG REQUIRED)
add_executable(demo_reader2 demo_reader2.cpp)
