cmake_minimum_required(VERSION 3.15)
project(opengl)

set(ASSIMP_BUILD_SHARED_LIBS, TRUE)
add_subdirectory(assimp)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(glew/build/cmake)
include_directories(CImg)
include_directories(glew/include)
add_compile_definitions(STB_IMAGE_IMPLEMENTATION)

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

find_package(OpenGL REQUIRED)
target_include_directories(opengl PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(opengl glm glfw glew pthread assimp ${OPENGL_gl_LIBRARY} )
