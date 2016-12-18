cmake_minimum_required(VERSION 2.8)
project(cuadros)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_C_FLAGS "-pedantic -Wall -std=c99")
  set(CMAKE_CXX_FLAGS "-pedantic -Wall -std=c++11")

  if (USE_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  endif()
endif()
message("FLAGS: ${CMAKE_CXX_FLAGS}")

# For ease of development, I'm guessing a shortcut to Qt is installed.
set(CMAKE_PREFIX_PATH "$ENV{HOME}/tarballs/qtroot")

find_package(OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

add_subdirectory(twodee)
add_definitions(${TWODEE_DEFINITIONS})

set(CUADROS_EXECUTABLE "Cuadros")

add_executable (
  ${CUADROS_EXECUTABLE}
  cuadros.cpp
  CuadrosCanvas.cpp
  CuadrosRenderer.cpp
  CuadrosWindow.cpp
)
set_target_properties(${CUADROS_EXECUTABLE} PROPERTIES MACOSX_BUNDLE TRUE)
if(NOT APPLE)
  set_target_properties(${CUADROS_EXECUTABLE} PROPERTIES OUTPUT_NAME cuadros)
endif()
include_directories(${CUADROS_EXECUTABLE} ${CMAKE_BINARY_DIR}/include twodee/include ${TWODEE_INCLUDE_DIRECTORIES} ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(${CUADROS_EXECUTABLE} Qt5::Widgets Qt5::OpenGL ${TWODEE_LIBRARIES} ${OPENGL_LIBRARIES})
