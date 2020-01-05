#include <array>
#include "material.h"
#include "errors.h"

TMaterial::TMaterial(TMaterialBuilder &&builder)
    : Textures(std::move(builder.Textures_))
      , CubeTextures(std::move(builder.CubeTextures_))
      , Colors(std::move(builder.Colors_))
      , Constants(std::move(builder.Constants_)) {
}

TMaterial::TMaterial(TMaterial &&src) noexcept {
    Textures = std::move(src.Textures);
    CubeTextures = std::move(src.CubeTextures);
    Colors = std::move(src.Colors);
    Constants = std::move(src.Constants);
    src.Textures.clear();
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

