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
#include "shaders/depth.h"
#include "shaders/hdr.h"
#include "cube.h"
#include "framebuffer.h"
#include "scene_setup.h"
#include "shader_set.h"
#include <glm/glm.hpp>
#include <random>
#include <utility>

class TScene {
private:
    TUniformBinding<TProjectionView> ProjectionView;
    TUniformBinding<TLights> LightSetup;
    TUniformBinding<TLightsPos> LightsPos;
    TUniformBuffer Connector{&ProjectionView, &LightSetup, &LightsPos};
    TSceneShader SceneShader{ProjectionView, LightSetup, LightsPos};
    TLightShader LightShader{ProjectionView};
    TShadowShader ShadowShader{};
    TDepthShader DepthShader{};
    THdrShader HdrShader{};
    TSilhouetteShader SilhouetteShader{ProjectionView};
    TBorderShader BorderShader{};
    TSkyboxShader SkyboxShader{ProjectionView};
    TParticlesShader ParticlesShader{ProjectionView, LightSetup, LightsPos};
    TNormalsShader NormalsShader{ProjectionView};
    std::vector<std::tuple<glm::vec3, glm::vec3>> Particles{ 2000 };
    TParticleInjector Injector;
    int CurrentParticles = 0;
    float ExplosionTime = 0;

    TFlatTexture AsphaltTex{
        TTextureBuilder()
            .SetFile("images/asphalt_diffuse.png")
            .SetUsage(ETextureUsage::SRgb)
            .SetMipmap(ETextureMipmap::Linear)};
    TFlatTexture AsphaltBump{
        TTextureBuilder()
            .SetFile("images/asphalt_height.png")
            .SetUsage(ETextureUsage::Normals)
            .SetMipmap(ETextureMipmap::Linear)
    };

    TCubeTexture SkyTex{
        TCubeTextureBuilder()
            .SetUsage(ETextureUsage::SRgb)
            .SetPosX("images/skybox/right.jpg")
            .SetNegX("images/skybox/left.jpg")
            .SetPosY("images/skybox/top.jpg")
            .SetNegY("images/skybox/bottom.jpg")
            .SetPosZ("images/skybox/front.jpg")
            .SetNegZ("images/skybox/back.jpg")};

    TMaterial Asphalt{
        TMaterialBuilder()
            .SetColor(EMaterialProp::Specular, glm::vec4(0.03f, 0.03f, 0.03f, 1.0f))
            .SetTexture(EMaterialProp::Diffuse, AsphaltTex)
            .SetTexture(EMaterialProp::Normal, AsphaltBump)
            .SetConstant(EMaterialProp::Shininess, 8)};
    TMaterial Container{
        TMaterialBuilder()
            .SetTexture(EMaterialProp::Diffuse,
                        TTextureBuilder()
                            .SetUsage(ETextureUsage::SRgb)
                            .SetFile("images/container2_diffuse.png"))
            .SetTexture(EMaterialProp::Specular,
                        TTextureBuilder()
                            .SetUsage(ETextureUsage::SRgb)
                            .SetFile("images/container2_specular.png"))
            .SetTexture(EMaterialProp::Height,
                        TTextureBuilder()
                            .SetUsage(ETextureUsage::Height)
                            .SetFile("images/container2_specular2.png"))
            .SetTexture(EMaterialProp::Normal,
                        TTextureBuilder()
                            .SetUsage(ETextureUsage::Normals)
                            .SetFile("images/container2_specular2.png"))
            .SetConstant(EMaterialProp::Reflection, .01)
            .SetConstant(EMaterialProp::Shininess, 64)};
    TMesh Sky{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, Cube<false, false>(
                TGeomBuilder().SetSize(1).SetBackward(true)))
            .AddLayout(EDataType::Float, 3)};
    TMaterial
        Grass{TMaterialBuilder().SetTexture(EMaterialProp::Diffuse,
                                            TTextureBuilder()
                                                .SetUsage(ETextureUsage::SRgba)
                                                .SetFile("images/grass.png"))};
    TMaterial
        Window{TMaterialBuilder().SetTexture(EMaterialProp::Diffuse,
                                             TTextureBuilder()
                                                 .SetUsage(ETextureUsage::SRgba)
                                                 .SetFile("images/window.png"))};
    TMesh GroundCube{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, Cube<true, true, true>(TGeomBuilder().SetTextureMul(10, 10)))
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 2)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)};
    TMesh SimpleCube{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, Cube<true, true, true>())
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 2)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)};

    TMesh QuadPoly{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, DoubleQuad())
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 3)
            .AddLayout(EDataType::Float, 2)};

    TMesh ScreenQuad{
        TMeshBuilder()
            .SetVertices(EBufferUsage::Static, Quad<false, true>(
                TGeomBuilder()
                    .SetTextureMul(1, -1)
                    .SetTextureOffset(0, 1)
                    .SetSize(1.0f)))
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

    glm::vec3 Directional{0.6f, -1.0f, 1.0f};
    float SpotAngle = 0;
    std::array<std::pair<glm::vec3, glm::vec3>, 2> Spots = {
        std::make_pair(glm::vec3{-4.0f, 13.0f, -6.0f}, glm::vec3{2.0f, 0.2f, 0.1f}),
        std::make_pair(glm::vec3{2.0f, 2.0f, 40.0f}, glm::vec3{2.0f, 1.0f, 0.1f})
    };

    using OpaqueType = std::tuple<glm::vec3, glm::mat4, TMaterial &, TMesh &>;
    std::array<OpaqueType, 3> OpaqueObjects = {
        std::forward_as_tuple(glm::vec3(-18, 4, 34), NConstMath::RotateY4(20) * NConstMath::Scale(8), Grass, QuadPoly),
        std::forward_as_tuple(glm::vec3(-28, 6, 30), NConstMath::RotateY4(30) * NConstMath::Scale(8), Window, QuadPoly),
        std::forward_as_tuple(glm::vec3(-48, 5, 44), NConstMath::RotateY4(40) * NConstMath::Scale(10), Window, QuadPoly)
    };

    TFrameBuffer FrameBuffer;
    TFrameBuffer AliasedFrameBuffer;
    TFrameBuffer GlobalLightShadow{
        false,
        TTextureBuilder()
            .SetEmpty(4096, 4096)
            .SetMagLinear(false)
            .SetMinLinear(false)
            .SetWrap(ETextureWrap::ClampToBorder)
            .SetBorderColor(glm::vec4(1))
            .SetUsage(ETextureUsage::Depth)};
    TFrameBuffer SpotLightShadow{
        false,
        TCubeTextureBuilder()
            .SetEmpty(512, 512, 512)
            .SetMagLinear(false)
            .SetMinLinear(false)
            .SetUsage(ETextureUsage::Depth)};
    TFrameBuffer SpotLightShadow2{
        false,
        TCubeTextureBuilder()
            .SetEmpty(512, 512, 512)
            .SetMagLinear(false)
            .SetMinLinear(false)
            .SetUsage(ETextureUsage::Depth)};

public:
    TScene(int width, int height)
        : FrameBuffer(
        TTextureBuilder().SetEmpty(width, height).SetWrap(ETextureWrap::ClampToEdge),
        TTextureBuilder().SetEmpty(width, height).SetWrap(ETextureWrap::ClampToEdge).SetUsage(ETextureUsage::Depth))
          , AliasedFrameBuffer(
            TRenderBuffer(ETextureUsage::Rgba, width, height, 4),
            TRenderBuffer(ETextureUsage::Depth, width, height, 4)) {
    }

    void Draw(glm::mat4 project, glm::mat4 view, glm::vec3 position, float interval, bool useMap);

private:
    void DrawScene(IShaderSet &&set);
    void SetupLights(glm::vec3 position, float interval);
    void UpdateFountain(float interval);
    void DrawFountain(IShaderSet &set);
    void DrawSkybox();
    void DrawObjects(IShaderSet &set);
    void DrawOpaques(IShaderSet &set);
    void DrawLightCubes();
    void DrawBorder();
};
