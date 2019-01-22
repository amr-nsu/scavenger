cmake_minimum_required(VERSION 2.8)
project(detect)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(
    "-Wall" "-Wextra"
    "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
    "$<$<CONFIG:RELEASE>:-O3>"
)

set(SRC_LIST
    main.cpp
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
