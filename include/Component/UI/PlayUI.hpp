#ifndef COMPONENT_UI_PLAYUI_HPP
#define COMPONENT_UI_PLAYUI_HPP

#include <functional>
#include <memory>

#include "Component/IStateful.hpp"
#include "Component/Player/Player.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

class PlayUI : public Util::GameObject, public IStateful {
public:
    using HudStateProvider = std::function<PlayerHudState()>;

    explicit PlayUI(HudStateProvider hudStateProvider);

    void Update() override;

private:
    void UpdateLayout();
    void SyncHudState();
    bool IsHudStateDirty(const PlayerHudState &state) const;

    HudStateProvider m_HudStateProvider;
    std::shared_ptr<Util::Image> m_Background;
    std::shared_ptr<Util::Text> m_HealthText;
    std::shared_ptr<Util::Text> m_ShieldText;
    std::shared_ptr<Util::Text> m_AmmoText;
    std::shared_ptr<Util::GameObject> m_HealthTextNode;
    std::shared_ptr<Util::GameObject> m_ShieldTextNode;
    std::shared_ptr<Util::GameObject> m_AmmoTextNode;
    PlayerHudState m_LastHudState = {};
    bool m_HasLastHudState = false;
};

#endif
