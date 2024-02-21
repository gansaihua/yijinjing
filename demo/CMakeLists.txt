link_libraries(yijinjing)

add_executable(demo_writer demo_writer.cpp)
add_executable(demo_reader demo_reader.cpp)

find_package(eventpp CONFIG REQUIRED)
add_executable(demo2_writer demo2_writer.cpp)
add_executable(demo2_reader demo2_reader.cpp)
