cmake_minimum_required(VERSION 3.26) # common to every CLion project
project(untitled) # project name

add_executable(untitled main.c
        lib/model.h
        lib/model.c
        lib/mountain.h
        lib/mountain.c
        lib/textures.c
        lib/textures.h)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(untitled -lm -lGL -lglut ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )