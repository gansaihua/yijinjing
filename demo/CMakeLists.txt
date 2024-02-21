find_package(fmt REQUIRED)
link_libraries(fmt::fmt yijinjing)

add_executable(demo_writer demo_writer.cpp)
add_executable(demo_reader demo_reader.cpp)

add_executable(demo2_writer demo2_writer.cpp)
add_executable(demo2_reader demo2_reader.cpp)
