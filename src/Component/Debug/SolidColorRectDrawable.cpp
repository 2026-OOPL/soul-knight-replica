#include "Component/Debug/SolidColorRectDrawable.hpp"

#include <array>
#include <string>

#include "Util/Logger.hpp"
#include "config.hpp"

namespace {

const std::string kBaseVertexShaderPath =
    std::string(RESOURCE_DIR) + "/../PTSD/assets/shaders/Base.vert";
const std::string kBaseFragmentShaderPath =
    std::string(RESOURCE_DIR) + "/../PTSD/assets/shaders/Base.frag";

std::array<Uint8, 4> BuildPixel(const Util::Color &color) {
    return {
        static_cast<Uint8>(color.r),
        static_cast<Uint8>(color.g),
        static_cast<Uint8>(color.b),
        static_cast<Uint8>(color.a)
    };
}

} // namespace

SolidColorRectDrawable::SolidColorRectDrawable(
    const Util::Color &color,
    bool useAA
)
    : m_Color(color) {
    if (s_Program == nullptr) {
        InitProgram();
    }
    if (s_VertexArray == nullptr) {
        InitVertexArray();
    }

    m_UniformBuffer = std::make_unique<Core::UniformBuffer<Core::Matrices>>(
        *s_Program,
        "Matrices",
        0
    );

    const std::array<Uint8, 4> pixel = BuildPixel(this->m_Color);
    this->m_Texture = std::make_unique<Core::Texture>(
        GL_RGBA,
        1,
        1,
        pixel.data(),
        useAA
    );
}

void SolidColorRectDrawable::Draw(const Core::Matrices &data) {
    this->m_UniformBuffer->SetData(0, data);

    this->m_Texture->Bind(UNIFORM_SURFACE_LOCATION);
    s_Program->Bind();
    s_Program->Validate();

    s_VertexArray->Bind();
    s_VertexArray->DrawTriangles();
}

void SolidColorRectDrawable::SetColor(const Util::Color &color) {
    this->m_Color = color;
    this->ApplyColor();
}

void SolidColorRectDrawable::InitProgram() {
    s_Program = std::make_unique<Core::Program>(
        kBaseVertexShaderPath,
        kBaseFragmentShaderPath
    );
    s_Program->Bind();

    const GLint location = glGetUniformLocation(s_Program->GetId(), "surface");
    glUniform1i(location, UNIFORM_SURFACE_LOCATION);
}

void SolidColorRectDrawable::InitVertexArray() {
    s_VertexArray = std::make_unique<Core::VertexArray>();

    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            -0.5F, 0.5F,
            -0.5F, -0.5F,
            0.5F, -0.5F,
            0.5F, 0.5F,
        },
        2
    ));

    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            0.0F, 0.0F,
            0.0F, 1.0F,
            1.0F, 1.0F,
            1.0F, 0.0F,
        },
        2
    ));

    s_VertexArray->SetIndexBuffer(
        std::make_unique<Core::IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2,
            0, 2, 3,
        })
    );
}

void SolidColorRectDrawable::ApplyColor() {
    if (this->m_Texture == nullptr) {
        LOG_ERROR("SolidColorRectDrawable texture is not initialized.");
        return;
    }

    const std::array<Uint8, 4> pixel = BuildPixel(this->m_Color);
    this->m_Texture->UpdateData(GL_RGBA, 1, 1, pixel.data());
}

std::unique_ptr<Core::Program> SolidColorRectDrawable::s_Program = nullptr;
std::unique_ptr<Core::VertexArray> SolidColorRectDrawable::s_VertexArray = nullptr;
