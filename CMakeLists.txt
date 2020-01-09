cmake_minimum_required(VERSION 3.15)
project(opengl)
add_subdirectory(soil)
include_directories(soil/inc)
include_directories(glm)

set(CMAKE_CXX_STANDARD 17)
set(GLEW_USE_STATIC_LIBS TRUE)
add_executable(opengl
        main.cpp
        buffer.h
        buffer.cpp
        common.h
        errors.h
        errors.cpp
        shader_program.h
        shader_program.cpp
        mesh.h
        mesh.cpp
        model.h
        model.cpp
        material.h
        material.cpp
        texture.h
        texture.cpp
        model_loader.h
        model_loader.cpp
        scene.h
        scene.cpp
        scene_setup.h
        shader_set.h
        shader_set.cpp
        framebuffer.h
        framebuffer.cpp
        uniform_buffer.h
        uniform_buffer.cpp
        shaders/light.h
        shaders/scene.h
        shaders/border.h
        shaders/silhouette.h
        )

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
set(ASSIMP_BUILD_SHARED_LIBS, TRUE)

find_package(assimp REQUIRED)
find_library(CORE CoreFoundation REQUIRED)
target_include_directories(opengl PUBLIC ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS})
find_library(ASSIMP ${ASSIMP_LIBRARIES})
target_link_libraries(opengl glfw GLEW::glew ${OPENGL_gl_LIBRARY} soil pthread ${ASSIMP} ${CORE})
