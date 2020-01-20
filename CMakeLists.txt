cmake_minimum_required(VERSION 3.15)
project(opengl)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(assimp)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(glew/build/cmake)
include_directories(CImg)
include_directories(glew/include)
include_directories(stb)
add_compile_definitions(STB_IMAGE_IMPLEMENTATION)

function(ADD_RESOURCES out_var root)
    set(result)
    set(sources "namespace NResource {\n    namespace NImpl { extern \"C\" {\n")
    set(defines)
    foreach(in_f ${ARGN})
        file(RELATIVE_PATH src_f "${CMAKE_CURRENT_SOURCE_DIR}/${root}" "${CMAKE_CURRENT_SOURCE_DIR}/${in_f}")
        string(REPLACE "/" "_" fs_flat ${src_f})
        string(REPLACE "." "_" fs_flat ${fs_flat})
        set(out_f "${PROJECT_BINARY_DIR}/${fs_flat}.c")
        add_custom_command(OUTPUT ${out_f}
                COMMAND xxd -i ${src_f} ${out_f}
                DEPENDS ${in_f}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${root}"
                COMMENT "Building GLSL object ${out_f}"
                VERBATIM
                )
        list(APPEND sources "        extern unsigned char ${fs_flat}\;\n        extern unsigned int ${fs_flat}_len\;\n")
        list(APPEND defines "    const TResource ${fs_flat}(&NImpl::${fs_flat}, NImpl::${fs_flat}_len)\;\n")
        list(APPEND result ${out_f})
    endforeach()
    list(APPEND sources "    }}\n")
    list(APPEND defines "}\n")
    file(WRITE "${PROJECT_BINARY_DIR}/resource_def.h" ${sources}${defines})
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

add_resources(RES src
        src/shaders/blur.frag
        src/shaders/border.frag
        src/shaders/border.vert
        src/shaders/depth.frag
        src/shaders/light.frag
        src/shaders/light.vert
        src/shaders/normals.frag
        src/shaders/normals.geom
        src/shaders/normals.vert
        src/shaders/particles.frag
        src/shaders/particles.vert
        src/shaders/scene.frag
        src/shaders/scene.geom
        src/shaders/scene.vert
        src/shaders/shadow.frag
        src/shaders/shadow.geom
        src/shaders/shadow.vert
        src/shaders/silhouette.frag
        src/shaders/silhouette.vert
        src/shaders/skybox.frag
        src/shaders/skybox.vert
        )



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
    ${RES}
    )

find_package(OpenGL REQUIRED)
target_include_directories(opengl PUBLIC ${PROJECT_BINARY_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(opengl glm glfw glew_s assimp ${OPENGL_gl_LIBRARY})

add_compile_options(-fvisibility=hidden)
