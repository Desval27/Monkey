cmake_minimum_required(VERSION 3.10)
project(Monkey)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB monkey_SOURCE "./src/*.cpp")

set(examples_DIR ${CMAKE_SOURCE_DIR}/examples)

add_library(Monkey STATIC ${monkey_SOURCE})
target_include_directories(Monkey PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(Monkey PUBLIC USE_DEBUG)

add_executable(music_demo ${examples_DIR}/music_demo.cpp)
target_include_directories(music_demo PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(music_demo PUBLIC USE_DEBUG)
target_link_libraries(music_demo PUBLIC Monkey)

add_executable(plot_things ${examples_DIR}/plot_things.cpp)
target_include_directories(plot_things PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(plot_things PUBLIC USE_DEBUG)
target_link_libraries(plot_things PUBLIC Monkey)

find_program(IWYU_PATH NAMES include-what-you-use iwyu)
if(IWYU_PATH)
    set_target_properties(Monkey PROPERTIES
        CXX_INCLUDE_WHAT_YOU_USE "${IWYU_PATH};-Xiwyu;--mapping_file=${CMAKE_SOURCE_DIR}/iwyu.imp"
    )
endif()

