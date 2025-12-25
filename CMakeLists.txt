cmake_minimum_required(VERSION 3.20)
project (logger LANGUAGES CXX C)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_C_FLAGS "$ -D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE")
set (CMAKE_CXX_FLAGS "-D_CRT_SECURE_NO_WARNINGS=1")

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.*pp)
set(PLUGIN_SOURCES ${PLUGIN_SOURCES} ${SRCS})
set(PLUGIN_INCLUDES ${PLUGIN_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/sources/)

add_subdirectory(sources/buffers)
add_subdirectory(sources/sinks)

set(SOURCES
    sources/main.cpp
)

message("==== ${PLUGIN_SOURCES}")
message("==== ${PLUGIN_INCLUDES}")

add_executable(${PROJECT_NAME} ${SOURCES} ${PLUGIN_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PLUGIN_INCLUDES}
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE)