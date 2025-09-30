cmake_minimum_required(VERSION 3.5)

project(JCFind VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        chain.cpp
        crossword.cpp
        field.cpp
        finder.cpp
        chain.h
        crossword.h
        field.h
        finder.h
)

add_executable(JCFind
    ${PROJECT_SOURCES}
)

if (MSVC)
    target_compile_options(JCFind PUBLIC /W4)
else ()
    target_compile_options(JCFind PUBLIC -Wall -Wextra -Wpedantic)
endif ()

target_link_libraries(JCFind PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
