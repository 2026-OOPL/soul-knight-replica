#ifndef SLIDER_HPP
#define SLIDER_HPP

#include <functional>
#include <memory>

#include "Component/Button/ImageButton.hpp"
#include "Component/IStateful.hpp"
#include "Util/GameObject.hpp"

class Slider : public Util::GameObject, public IStateful{
public:
    Slider(
        std::function<float()> getValue,
        std::function<void(float)> setValue,
        std::shared_ptr<ImageButtonTheme> sliderKnobImage,
        std::string sliderActiveImage,
        glm::vec2 sliderStart,
        glm::vec2 sliderEnd
    );

    void Update() override;

private:
    std::function<float()> m_GetValue;
    std::function<void(float)> m_SetValue;

    std::shared_ptr<ImageButton> m_Knob;
    std::shared_ptr<Util::GameObject> m_Active;

    glm::vec2 m_SliderStart;
    glm::vec2 m_SliderEnd;

    float GetCursorPercentage();
    void SetSliderByPercentage(float percentage);

    float m_SliderLength;

};

#endif