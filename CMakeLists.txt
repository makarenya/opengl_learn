cmake_minimum_required(VERSION 3.15)
project(opengl)

set(ASSIMP_BUILD_SHARED_LIBS, TRUE)
add_subdirectory(assimp)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(glew/build/cmake)
include_directories(CImg)
include_directories(glew/include)
include_directories(stb)
add_compile_definitions(STB_IMAGE_IMPLEMENTATION)

set(CMAKE_CXX_STANDARD 17)
set(GLEW_USE_STATIC_LIBS TRUE)
add_executable(opengl
        src/main.cpp
        src/buffer.h
        src/buffer.cpp
        src/common.h
        src/errors.h
        src/errors.cpp
        src/shader_program.h
        src/shader_program.cpp
        src/mesh.h
        src/mesh.cpp
        src/model.h
        src/model.cpp
        src/material.h
        src/material.cpp
        src/texture.h
        src/texture.cpp
        src/model_loader.h
        src/model_loader.cpp
        src/scene.h
        src/scene.cpp
        src/scene_setup.h
        src/shader_set.h
        src/shader_set.cpp
        src/framebuffer.h
        src/framebuffer.cpp
        src/uniform_buffer.h
        src/uniform_buffer.cpp
        src/shaders/light.h
        src/shaders/scene.h
        src/shaders/border.h
        src/shaders/silhouette.h
        )

find_package(OpenGL REQUIRED)
target_include_directories(opengl PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(opengl glm glfw glew pthread assimp ${OPENGL_gl_LIBRARY} )

add_compile_options(-fvisibility=hidden)
