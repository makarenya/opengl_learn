#include "model_loader.h"
#include "uniform_buffer.h"
#include "shaders/scene.h"
#include "shaders/light.h"
#include "shaders/border.h"
#include "shaders/silhouette.h"
#include "shaders/particles.h"
#include "shaders/skybox.h"
#include "shaders/normals.h"
#include "shaders/blur.h"
#include "cube.h"
#include "framebuffer.h"
#include <glm/glm.hpp>

struct TProjectionView {
    glm::mat4 projection;
    glm::mat4 view;
};

struct TDirectionalLight {
    glm::vec4 Direction;
    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    TDirectionalLight() = default;
    TDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        : Direction(direction, 1.0)
          , Ambient(ambient, 1.0)
          , Diffuse(diffuse, 1.0)
          , Specular(specular, 1.0) {
    }
};

struct TSpotLight {
    glm::vec4 Position;
    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec3 Specular;
    float Linear;
    float Quadratic;
    float tmp[3];
    TSpotLight() = default;
    TSpotLight(glm::vec3 position,
               glm::vec3 ambient,
               glm::vec3 diffuse,
               glm::vec3 specular,
               float linear,
               float quadratic)
        : Position(position, 1.0)
          , Ambient(ambient, 1.0)
          , Diffuse(diffuse, 1.0)
          , Specular(specular)
          , Linear(linear)
          , Quadratic(quadratic) {
    }
};

struct TProjectorLight {
    glm::vec4 Position;
    glm::vec4 Target;

    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec3 Specular;

    float InnerCutoff;
    float OuterCutoff;
    float Linear;
    float Quadratic;
    float tmp;
    TProjectorLight() = default;
    TProjectorLight(glm::vec3 position,
                    glm::vec3 target,
                    glm::vec3 ambient,
                    glm::vec3 diffuse,
                    glm::vec3 specular,
                    float innerCutoff,
                    float outerCutoff,
                    float linear,
                    float quadratic)
        : Position(position, 1.0f)
          , Target(target, 1.0f)
          , Ambient(ambient, 1.0f)
          , Diffuse(diffuse, 1.0f)
          , Specular(specular)
          , InnerCutoff(std::cos(glm::radians(innerCutoff)))
          , OuterCutoff(std::cos(glm::radians(outerCutoff)))
          , Linear(linear)
          , Quadratic(quadratic) {
    }
};

struct TLights {
    TDirectionalLight directional;
    TSpotLight spots[8];
    TProjectorLight projector;
    uint32_t spotCount;
};

class TScene {
private:
    TUniformBinding<TProjectionView> ProjectionView;
    TUniformBinding<TLights> LightSetup;
    TUniformBuffer Connector{&ProjectionView, &LightSetup};
    TSceneShader SceneShader{ProjectionView, LightSetup};
    TLightShader LightShader{ProjectionView};
    TSilhouetteShader SilhouetteShader{ProjectionView};
    TBorderShader BorderShader{};
    TSkyboxShader SkyboxShader{ProjectionView};
    TParticlesShader ParticlesShader{ProjectionView, LightSetup};
    TNormalsShader NormalsShader{ProjectionView};
    std::array<std::tuple<glm::vec3, glm::vec3>, 2000> Particles;
    int CurrentParticles = 0;
    double LastParticleTime = NAN;
    const glm::ivec3 Maxims{53, 37, 47};
    glm::ivec3 Currents{};
    float ExplosionCounter = 0;
    float ExplosionTime = 0;

    TTexture AsphaltTex{
        TTextureBuilder()
            .SetFile("images/asphalt_diffuse.png")
            .SetMipmap(ETextureMipmap::Linear)};
    TTexture SkyTex{
        TCubeTextureBuilder()
            .SetPosX("images/yokohama3/posx.jpg")
            .SetNegX("images/yokohama3/negx.jpg")
            .SetPosY("images/yokohama3/posy.jpg")
            .SetNegY("images/yokohama3/negy.jpg")
            .SetPosZ("images/yokohama3/posz.jpg")
            .SetNegZ("images/yokohama3/negz.jpg")};

    TMaterial Asphalt{
        TMaterialBuilder()
            .SetColor(EMaterialProp::Specular, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f))
            .SetTexture(EMaterialProp::Diffuse, AsphaltTex)
            .SetConstant(EMaterialProp::Shininess, 5)};
    TMaterial Container{
        TMaterialBuilder()
            .SetTexture(EMaterialProp::Diffuse, TTextureBuilder().SetFile("images/container2_diffuse.png"))
            .SetTexture(EMaterialProp::Specular, TTextureBuilder().SetFile("images/container2_specular.png"))
            .SetConstant(EMaterialProp::Reflection, .1)
            .SetConstant(EMaterialProp::Shininess, 64)};
    TMesh Sky{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, Cube<false, false>(
                TGeomBuilder().SetSize(1).SetBackward(true)))
            .AddLayout(EDataType::Float, 3)};
    TMaterial
        Grass{TMaterialBuilder().SetTexture(EMaterialProp::Diffuse, TTextureBuilder().SetFile("images/grass.png"))};
    TMaterial
        Window{TMaterialBuilder().SetTexture(EMaterialProp::Diffuse, TTextureBuilder().SetFile("images/window.png"))};
    TMesh GroundCube{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, Cube(TGeomBuilder().SetTextureMul(10, 10)))
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 2)};
    TMesh SimpleCube{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, Cube())
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 2)};

    TMesh QuadPoly{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, DoubleQuad())
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 2)};

    TModel Suit{LoadMesh("nanosuit/nanosuit.obj")};
    TModel Drop{LoadMesh("images/drop.obj")};
    TMesh Points{Drop.GetMesh(0),
                 TInstanceMeshBuilder()
                     .SetInstances(TArrayBuffer(EBufferUsage::Stream, nullptr, sizeof(float) * 6 * Particles.size()),
                                   6 * Particles.size())
                     .AddLayout(EDataType::Float, 3, 1)
                     .AddLayout(EDataType::Float, 3, 1)};
    std::array<std::pair<glm::vec3, glm::vec3>, 2> Spots = {
        std::forward_as_tuple(glm::vec3{-4.0f, 13.0f, 2.0f}, glm::vec3{2.0f, 0.2f, 0.1f}),
        std::forward_as_tuple(glm::vec3{2.0f, 2.0f, 40.0f}, glm::vec3{2.0f, 1.0f, 0.1f})
    };

    using OpaqueType = std::tuple<glm::vec3, glm::mat4, TMaterial &, TMesh &>;
    std::array<OpaqueType, 3> OpaqueObjects = {
        std::forward_as_tuple(glm::vec3(-18, 4, 34), NConstMath::RotateY4(20) * NConstMath::Scale(8), Grass, QuadPoly),
        std::forward_as_tuple(glm::vec3(-28, 6, 30), NConstMath::RotateY4(30) * NConstMath::Scale(8), Window, QuadPoly),
        std::forward_as_tuple(glm::vec3(-48, 5, 44), NConstMath::RotateY4(40) * NConstMath::Scale(10), Window, QuadPoly)
    };

    TFrameBuffer FrameBuffer;
    TFrameBuffer AliasedFrameBuffer;

public:
    TScene(int width, int height)
        : FrameBuffer(width, height, 0, true)
          , AliasedFrameBuffer(width, height, 4, true) {
    }

    void Draw(glm::mat4 project, glm::mat4 view, glm::vec3 position);

private:
    void SetupLights(glm::vec3 position);
    void DrawSkybox();
    void DrawObjects(glm::vec3 position);
    void DrawOpaques(glm::vec3 position);
    void DrawLightCubes();
    void DrawBorder();
};
