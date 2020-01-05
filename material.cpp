#include "material.h"
#include "errors.h"

TMaterial::TMaterial(const TMaterialBuilder &builder)
    : Textures(builder.Textures_)
      , CubeTextures(builder.CubeTextures_)
      , Colors(builder.Colors_)
      , Constants(builder.Constants_) {
}

TTextureBinder TMaterial::Bind(TProgramSetup &setup) const {
    TTextureBinder binder;
    for (auto &texture : Textures) {
        setup.TrySet("material." + texture.first, binder.Attach(texture.second));
    }
    for (auto &texture : CubeTextures) {
        setup.TrySet("material." + texture.first, binder.Attach(texture.second));
    }
    for (auto &color : Colors) {
        setup.TrySet("material." + color.first, color.second);
    }
    for (auto &constant : Constants) {
        setup.TrySet("material." + constant.first, constant.second);
    }
    return binder;
}

void TMaterial::Draw(TProgramSetup &setup, TMesh &mesh) {
    auto binder = Bind(setup);
    mesh.Draw();
}

