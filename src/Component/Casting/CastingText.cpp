#include <memory>

#include <glm/vec2.hpp>

#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"

#include "Component/Casting/CastingText.hpp"

CastingText::CastingText(std::string content) :
    CastingText(content, glm::vec2(0, 0)) {};

CastingText::CastingText(std::string content, glm::vec2 position) :
    CastingText(content, position, 32) {};

CastingText::CastingText(std::string content, glm::vec2 position, double size) {
    m_Text = std::make_shared<Util::Text>(
        RESOURCE_DIR"/Font/Cubic-Font/Cubic_11.ttf",
        size,
        content,
        Util::Color(255, 255, 255),
        true
    );

    this->SetAbsoluteTranslation(position);
    this->SetDrawable(m_Text);
}

Util::Transform CastingText::GetObjectTransform() const {
    return this->m_Transform;
}