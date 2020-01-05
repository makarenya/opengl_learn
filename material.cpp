#include "material.h"
#include "errors.h"

TMaterial::TMaterial(const TMaterialBuilder &builder)
    : Textures(builder.Textures_)
      , CubeTextures(builder.CubeTextures_)
      , Colors(builder.Colors_)
      , Constants(builder.Constants_) {
}

void TMaterial::Use(TProgramSetup &setup) const {
    setup.FlushTextures();
    for (auto &texture : Textures) {
        texture.second.Bind(setup.TryTextureLoc("material." + texture.first));
    }
    for (auto &texture : CubeTextures) {
        texture.second.Bind(setup.TryTextureLoc("material." + texture.first));
    }
    for (auto &color : Colors) {
        setup.TrySet("material." + color.first, color.second);
    }
    for (auto &constant : Constants) {
        setup.TrySet("material." + constant.first, constant.second);
    }
}

