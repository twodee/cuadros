cmake_minimum_required(VERSION 2.8)
project(cuadros)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# For ease of development, I'm guessing a shortcut to Qt is installed.
set(CMAKE_PREFIX_PATH "$ENV{HOME}/tarballs/qtroot")

find_package(OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

add_subdirectory(twodee)

add_executable (
  cuadros
  cuadros.cpp
  CuadrosCanvas.cpp
  CuadrosRenderer.cpp
  CuadrosWindow.cpp
)
include_directories(cuadros ${CMAKE_BINARY_DIR}/include twodee/include ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(cuadros Qt5::Widgets Qt5::OpenGL twodee ${OPENGL_LIBRARIES})

