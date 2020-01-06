#include "model_loader.h"
#include "uniform_buffer.h"
#include "shaders/scene.h"
#include "shaders/light.h"
#include "shaders/border.h"
#include "shaders/silhouette.h"
#include "shaders/skybox.h"
#include "shaders/blur.h"
#include "cube.h"
#include "framebuffer.h"
#include <glm/glm.hpp>

class TScene {
private:
    struct TProjectionView {
        glm::mat4 projection;
        glm::mat4 view;
    };

    TUniformBuffer<TProjectionView> ProjectionView;
    TUniformConnector Connector{{&ProjectionView}};
    TSceneShader SceneShader{};
    TLightShader LightShader{};
    TSilhouetteShader SilhouetteShader{};
    TBorderShader BorderShader{};
    TSkyboxShader SkyboxShader{};

    TTexture AsphaltTex{
        TTextureBuilder()
            .SetFile("images/asphalt_diffuse.png")
            .SetMipmap(ETextureMipmap::Linear)};
    TCubeTexture SkyTex{
        TCubeTextureBuilder()
            .SetPosX("images/yokohama3/posx.jpg")
            .SetNegX("images/yokohama3/negx.jpg")
            .SetPosY("images/yokohama3/posy.jpg")
            .SetNegY("images/yokohama3/negy.jpg")
            .SetPosZ("images/yokohama3/posz.jpg")
            .SetNegZ("images/yokohama3/negz.jpg")};

    TMaterial Asphalt{
        TMaterialBuilder()
            .SetColor("specular_col", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f))
            .SetTexture("diffuse_map", AsphaltTex)
            .SetConstant("shiness", 5)};
    TMaterial Container{
        TMaterialBuilder()
            .SetTexture("diffuse_map", TTextureBuilder().SetFile("images/container2_diffuse.png"))
            .SetTexture("specular_map", TTextureBuilder().SetFile("images/container2_specular.png"))
            .SetConstant("shiness", 64)};
    TMesh Sky{
        TMeshBuilder()
            .Vertices(EBufferUsage::Static, Cube<false, false>(
                TGeomBuilder().SetSize(1).SetBackward(true)))
            .Layout(EDataType::Float, 3)};
    TMaterial Skybox{TMaterialBuilder().SetTexture("skybox", SkyTex)};
    TMaterial Grass{TMaterialBuilder().SetTexture("diffuse_map", TTextureBuilder().SetFile("images/grass.png"))};
    TMaterial Window{TMaterialBuilder().SetTexture("diffuse_map", TTextureBuilder().SetFile("images/window.png"))};
    TMesh GroundCube{
        TMeshBuilder()
            .Vertices(EBufferUsage::Static, Cube(TGeomBuilder().SetTextureMul(10, 10)))
            .Layout(EDataType::Float, 3)
            .Layout(EDataType::Float, 3)
            .Layout(EDataType::Float, 2)};
    TMesh SimpleCube{
        TMeshBuilder()
            .Vertices(EBufferUsage::Static, Cube())
            .Layout(EDataType::Float, 3)
            .Layout(EDataType::Float, 3)
            .Layout(EDataType::Float, 2)};

    TMesh QuadPoly{
        TMeshBuilder()
            .Vertices(EBufferUsage::Static, DoubleQuad())
            .Layout(EDataType::Float, 3)
            .Layout(EDataType::Float, 3)
            .Layout(EDataType::Float, 2)};

    TModel Suit{LoadMesh("nanosuit/nanosuit.obj")};

    std::array<std::pair<glm::vec3, glm::vec3>, 2> Spots = {
        std::forward_as_tuple(glm::vec3{-4.0f, 13.0f, 2.0f}, glm::vec3{1.0f, 0.2f, 0.1f}),
        std::forward_as_tuple(glm::vec3{2.0f, 2.0f, 40.0f}, glm::vec3{2.0f, 1.0f, 0.1f})
    };

    using OpaqueType = std::tuple<glm::vec3, glm::mat4, TMaterial &, TMesh &>;
    std::array<OpaqueType, 3> OpaqueObjects = {
        std::forward_as_tuple(glm::vec3(-18, 4, 34), NConstMath::RotateY4(20) * NConstMath::Scale(8), Grass, QuadPoly),
        std::forward_as_tuple(glm::vec3(-28, 6, 30), NConstMath::RotateY4(30) * NConstMath::Scale(8), Window, QuadPoly),
        std::forward_as_tuple(glm::vec3(-48, 5, 44), NConstMath::RotateY4(40) * NConstMath::Scale(10), Window, QuadPoly)
    };

    TFrameBuffer FrameBuffer;

public:
    TScene(int width, int height)
        : FrameBuffer(width, height, true) {
        ProjectionView.Bind(SceneShader, "Matrices");
        ProjectionView.Bind(LightShader, "Matrices");
        ProjectionView.Bind(SilhouetteShader, "Matrices");
    }

    void Draw(glm::mat4 project, glm::mat4 view, glm::vec3 position);

private:
    void SetupLights(TSceneSetup& setup, glm::vec3 position);
    void DrawSkybox();
    void DrawObjects(glm::vec3 position);
    void DrawOpaques(glm::vec3 position);
    void DrawLightCubes();
    void DrawBorder();
};
