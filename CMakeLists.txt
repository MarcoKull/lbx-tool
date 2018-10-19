cmake_minimum_required(VERSION 2.6)
project(lbx-tool)

# compiler options
add_definitions(-std=c++11 -pipe -pedantic -Wall -Wconversion)

# library with header file
add_executable(lbx-tool src/tool.cpp)
install(
    TARGETS lbx-tool
    RUNTIME DESTINATION bin
)
target_link_libraries(lbx-tool -llbx)
