#pragma once
#include "common.h"
#include <map>
#include <utility>
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"

class TMaterial;

class TMaterialBuilder {
public:
    BUILDER_MAP(std::string, TTexture, Texture) {};
    BUILDER_MAP_ALT(std::string, TCubeTexture, Texture, CubeTextures_) {};
    BUILDER_MAP(std::string, glm::vec4, Color) {};
    BUILDER_MAP(std::string, float, Constant) {};
};

class TMaterial {
private:
    std::map<std::string, TTexture> Textures;
    std::map<std::string, TCubeTexture> CubeTextures;
    std::map<std::string, glm::vec4> Colors;
    std::map<std::string, float> Constants;

public:

    TMaterial(const TMaterialBuilder &builder);
    TTextureBinder Bind(TProgramSetup &setup) const;
    void Draw(TProgramSetup &setup, TMesh &mesh);
};


