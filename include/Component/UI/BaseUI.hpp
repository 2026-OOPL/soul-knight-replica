#ifndef BASE_UI_HPP
#define BASE_UI_HPP

#include "Component/Button/ImageButton.hpp"
#include "Component/IStateful.hpp"
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include <glm/vec2.hpp>
#include <memory>

class BaseUI : public Util::GameObject, public IStateful {
public:
    BaseUI(bool dimBackground = false);

    ~BaseUI() override = default;

    void Update() override;

    bool GetExitSignal();

protected:
    Util::ms_t m_LaunchedTime;
    bool m_ExitSignal = false;
    
private:
    float m_LastZInIndex;
    std::shared_ptr<Util::GameObject> m_BackgroundDim;
};

#endif