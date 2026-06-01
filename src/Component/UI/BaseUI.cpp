#include "Component/UI/BaseUI.hpp"
#include "Util/GameObject.hpp"

BaseUI::BaseUI(bool dimBackground) : Util::GameObject(nullptr, 0) {
    m_LastZInIndex = m_ZIndex;
    m_LaunchedTime = Util::Time::GetElapsedTimeMs();

    m_BackgroundDim = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/UI/layer_dim.png"
        ),
        m_ZIndex
    );

    if (dimBackground) { this->AddChild(m_BackgroundDim); }
}

void BaseUI::Update() {
    for (auto const& i : this->GetChildren()) {
        std::shared_ptr<IStateful> stateful = std::dynamic_pointer_cast<IStateful>(i);
        if (stateful) { stateful->Update(); }
    }

    if (m_LastZInIndex != m_ZIndex) {
        float zOffset = m_ZIndex - m_LastZInIndex;
        for (auto const& i : this->GetChildren()) {
            i->SetZIndex(i->GetZIndex() + zOffset);
        }
        m_LastZInIndex = m_ZIndex;
    }
}

bool BaseUI::GetExitSignal() {
    return this->m_ExitSignal;
}