cmake_minimum_required(VERSION 3.5)
project(HoughTransformCircleDetector)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES src/main.cpp src/window.cpp)

find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5OpenGL REQUIRED )

add_executable(HoughTransformCircleDetector ${SOURCE_FILES})

qt5_use_modules( HoughTransformCircleDetector Core Widgets Gui OpenGL )

