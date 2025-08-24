cmake_minimum_required(VERSION 3.16)
project(NetworkChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

qt6_standard_project_setup()

set(SOURCES
    main.cpp
    chatwindow.cpp
)

set(HEADERS
    chatwindow.h
)

qt6_add_executable(NetworkChat ${SOURCES} ${HEADERS})

target_link_libraries(NetworkChat Qt6::Core Qt6::Widgets Qt6::Network)

# Enable automoc
set_target_properties(NetworkChat PROPERTIES
    AUTOMOC ON
)