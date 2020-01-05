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
        auto name = "material." + texture.first;
        if (setup.Has(name)) {
            setup.Set(name, binder.Attach(texture.second));
        }
    }
    for (auto &texture : CubeTextures) {
        auto name = "material." + texture.first;
        if (setup.Has(name)) {
            setup.Set(name, binder.Attach(texture.second));
        }
    }
    for (auto &color : Colors) {
        auto name = "material." + color.first;
        if (setup.Has(name)) {
            setup.Set(name, color.second);
        }
    }
    for (auto &constant : Constants) {
        auto name = "material." + constant.first;
        if (setup.Has(name)) {
            setup.Set(name, constant.second);
        }
    }
    return binder;
}

void TMaterial::Draw(TProgramSetup &setup, TMesh &mesh) {
    auto binder = Bind(setup);
    mesh.Draw();
}

