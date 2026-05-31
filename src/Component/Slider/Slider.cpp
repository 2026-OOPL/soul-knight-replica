#include <memory>
#include <stdexcept>

#include "Component/Slider/Slider.hpp"
#include "Util/GameObject.hpp"
#include "Util/Input.hpp"

Slider::Slider(
    std::function<float()> getValue,
    std::function<void(float)> setValue,
    std::shared_ptr<ImageButtonTheme> sliderKnobImage,
    std::string sliderActiveImage,
    glm::vec2 sliderStart,
    glm::vec2 sliderEnd
) {
    if (sliderStart.y != sliderEnd.y) {
        throw std::runtime_error("Cannot construct a non-horizontal silder");
    }

    this->m_GetValue = getValue;
    this->m_SetValue = setValue;

    this->m_SliderStart = sliderStart;
    this->m_SliderEnd = sliderEnd;

    m_SliderLength = this->m_SliderEnd.x - this->m_SliderStart.x;

    m_Knob = std::make_shared<ImageButton>(
        nullptr,
        sliderKnobImage,
        nullptr
    );

    m_Knob->m_Transform.translation.y = this->m_SliderStart.y;

    m_Active = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            sliderActiveImage,
            false
        ),
        m_ZIndex+1
    );

    m_Active->m_Transform.translation.y = this->m_SliderStart.y;
    
    SetSliderByPercentage(this->m_GetValue());

    this->AddChild(this->m_Knob);
    this->AddChild(this->m_Active);
}

void Slider::Update() {
    if (m_Knob->GetButtonState() == ButtonState::PRESSED) {
        float percentage = this->GetCursorPercentage();
        this->SetSliderByPercentage(percentage);

        m_SetValue(percentage);
    }

    m_Knob->SetZIndex(m_ZIndex+1);
    m_Active->SetZIndex(m_ZIndex);
}

float Slider::GetCursorPercentage() {
    float cursorOffset = Util::Input::GetCursorPosition().x;
    
    if (cursorOffset < this->m_SliderStart.x) {
        return 0.0F;
    }

    if (cursorOffset > this->m_SliderEnd.x) {
        return 1.0F;
    }

    return (cursorOffset - this->m_SliderStart.x) / m_SliderLength;

}

void Slider::SetSliderByPercentage(float percentage) {
    float knobOffset = this->m_SliderStart.x + percentage * m_SliderLength;

    this->m_Knob->m_Transform.translation.x = knobOffset;

    this->m_Active->m_Transform.translation.x = (knobOffset - m_SliderStart.x) / 2 + m_SliderStart.x;
    this->m_Active->m_Transform.scale.x = percentage * m_SliderLength;
}