#ifndef COMPONENT_DEBUG_SOLID_COLOR_RECT_DRAWABLE_HPP
#define COMPONENT_DEBUG_SOLID_COLOR_RECT_DRAWABLE_HPP

#include <memory>

#include "Core/Drawable.hpp"
#include "Core/Program.hpp"
#include "Core/Texture.hpp"
#include "Core/UniformBuffer.hpp"
#include "Core/VertexArray.hpp"
#include "Util/Color.hpp"

class SolidColorRectDrawable : public Core::Drawable {
public:
    explicit SolidColorRectDrawable(
        const Util::Color &color,
        bool useAA = false
    );

    glm::vec2 GetSize() const override { return {1.0F, 1.0F}; }
    void Draw(const Core::Matrices &data) override;

    void SetColor(const Util::Color &color);

private:
    void InitProgram();
    void InitVertexArray();
    void ApplyColor();

    static constexpr int UNIFORM_SURFACE_LOCATION = 0;

    static std::unique_ptr<Core::Program> s_Program;
    static std::unique_ptr<Core::VertexArray> s_VertexArray;

    std::unique_ptr<Core::Texture> m_Texture;
    std::unique_ptr<Core::UniformBuffer<Core::Matrices>> m_UniformBuffer;
    Util::Color m_Color;
};

#endif
