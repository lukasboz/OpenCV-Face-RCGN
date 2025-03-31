cmake_minimum_required(VERSION 3.16)
project(OpenCVProject)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

# Get the absolute path of the project root
set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
add_compile_definitions(PROJECT_ROOT_DIR="${PROJECT_ROOT_DIR}")

set(COMMON_SRC
        src/FaceDetector.cpp
        src/FaceDetector.h
        src/FaceRecognizerWrapper.cpp
        src/FaceRecognizerWrapper.h
)

set(QT_SOURCES
        src/main.cpp        # This file now creates the QApplication and MainWindow
        src/mainwindow.cpp
        src/mainwindow.h
        src/pin.cpp
        src/pin.h
        src/EditProfile.cpp
        src/EditProfile.h
        src/facemanager.cpp
        src/facemanager.h
)

# Main executable that integrates OpenCV and Qt
add_executable(OpenCVProject ${QT_SOURCES} ${COMMON_SRC})
target_link_libraries(OpenCVProject PRIVATE ${OpenCV_LIBS} Qt5::Widgets)

# Training executable (if still needed)
add_executable(OpenCVProjectTrain src/training.cpp ${COMMON_SRC})
target_link_libraries(OpenCVProjectTrain PRIVATE ${OpenCV_LIBS})
