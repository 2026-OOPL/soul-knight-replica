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

    struct BossHudState {
        int hp = 0;
        int maxHp = 0;
        bool visible = false;
    };

    using BossHudStateProvider = std::function<BossHudState()>;

    explicit PlayUI(
        HudStateProvider hudStateProvider,
        BossHudStateProvider bossHudStateProvider = nullptr
    );

    void Update() override;

private:
    void UpdateLayout();
    void SyncHudState();
    void SyncBossHudState();
    bool IsHudStateDirty(const PlayerHudState &state) const;
    bool IsBossHudStateDirty(const BossHudState &state) const;

    HudStateProvider m_HudStateProvider;
    BossHudStateProvider m_BossHudStateProvider;
    std::shared_ptr<Util::Image> m_Background;
    std::shared_ptr<Util::Image> m_HealthBar;
    std::shared_ptr<Util::Image> m_ShieldBar;
    std::shared_ptr<Util::Image> m_AmmoBar;
    std::shared_ptr<Util::Image> m_BossHealthBar;
    std::shared_ptr<Util::Text> m_HealthText;
    std::shared_ptr<Util::Text> m_ShieldText;
    std::shared_ptr<Util::Text> m_AmmoText;
    std::shared_ptr<Util::Text> m_BossHealthText;
    std::shared_ptr<Util::GameObject> m_HealthBarNode;
    std::shared_ptr<Util::GameObject> m_ShieldBarNode;
    std::shared_ptr<Util::GameObject> m_AmmoBarNode;
    std::shared_ptr<Util::GameObject> m_BossHealthBarNode;
    std::shared_ptr<Util::GameObject> m_HealthTextNode;
    std::shared_ptr<Util::GameObject> m_ShieldTextNode;
    std::shared_ptr<Util::GameObject> m_AmmoTextNode;
    std::shared_ptr<Util::GameObject> m_BossHealthTextNode;
    PlayerHudState m_LastHudState = {};
    BossHudState m_LastBossHudState = {};
    bool m_HasLastHudState = false;
    bool m_HasLastBossHudState = false;
};

#endif
