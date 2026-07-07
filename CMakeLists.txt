cmake_minimum_required(VERSION 3.16)

project(flowing_time LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
)

configure_file(
    Roboto-Regular.ttf
    Roboto-Regular.ttf
    COPYONLY
)