cmake_minimum_required(VERSION 3.16)
project(website VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Gui)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/frontend DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(website
    main.cpp
    webserver/webserver.cpp
    utils/utils.cpp
)

target_link_libraries(website
    PRIVATE Qt6::Core
    PRIVATE Qt6::Network
    PRIVATE Qt6::Gui
)