#include "Component/UI/PlayUI.hpp"

#include <string>
#include <utility>

#include <glm/vec2.hpp>

#include "Core/Context.hpp"
#include "Util/Color.hpp"

namespace {

constexpr float kUiScale = 1.0F;
constexpr float kMarginX = 24.0F;
constexpr float kMarginY = 24.0F;
constexpr float kBackgroundZIndex = 100.0F;
constexpr float kTextZIndex = 101.0F;
constexpr int kFontSize = 8;

const std::string kPlayUiTexture = RESOURCE_DIR "/UI/Gameplay/panel.png";
const std::string kFontPath = RESOURCE_DIR "/Font/Cubic-Font/Cubic_11.ttf";

const Util::Color kHealthTextColor = Util::Color(255, 248, 231);
const Util::Color kShieldTextColor = Util::Color(235, 249, 255);
const Util::Color kAmmoTextColor = Util::Color(255, 232, 153);

constexpr glm::vec2 kHealthTextOffset = {39.0F, -8.0F};
constexpr glm::vec2 kShieldTextOffset = {39.0F, -19.0F};
constexpr glm::vec2 kAmmoTextOffset = {86.0F, -15.0F};

PlayerHudState BuildEmptyHudState() {
    return {0, 0, 0, 0, 0, 0};
}

std::string FormatValueText(int current, int maximum) {
    return std::to_string(current) + "/" + std::to_string(maximum);
}

} // namespace

PlayUI::PlayUI(HudStateProvider hudStateProvider)
    : GameObject(nullptr, kBackgroundZIndex),
      m_HudStateProvider(std::move(hudStateProvider)) {
    this->m_Background = std::make_shared<Util::Image>(kPlayUiTexture, false);
    this->SetDrawable(this->m_Background);
    this->m_Transform.scale = {kUiScale, kUiScale};

    this->m_HealthText = std::make_shared<Util::Text>(
        kFontPath,
        kFontSize,
        "0/0",
        kHealthTextColor,
        false
    );
    this->m_ShieldText = std::make_shared<Util::Text>(
        kFontPath,
        kFontSize,
        "0/0",
        kShieldTextColor,
        false
    );
    this->m_AmmoText = std::make_shared<Util::Text>(
        kFontPath,
        kFontSize,
        "Ammo 0/0",
        kAmmoTextColor,
        false
    );

    this->m_HealthTextNode = std::make_shared<Util::GameObject>(
        this->m_HealthText,
        kTextZIndex
    );
    this->m_ShieldTextNode = std::make_shared<Util::GameObject>(
        this->m_ShieldText,
        kTextZIndex
    );
    this->m_AmmoTextNode = std::make_shared<Util::GameObject>(
        this->m_AmmoText,
        kTextZIndex
    );

    this->AddChild(this->m_HealthTextNode);
    this->AddChild(this->m_ShieldTextNode);
    this->AddChild(this->m_AmmoTextNode);

    this->UpdateLayout();
    this->SyncHudState();
}

void PlayUI::Update() {
    this->UpdateLayout();
    this->SyncHudState();
}

void PlayUI::UpdateLayout() {
    const std::shared_ptr<Core::Context> context = Core::Context::GetInstance();
    const float halfWindowWidth = static_cast<float>(context->GetWindowWidth()) / 2.0F;
    const float halfWindowHeight = static_cast<float>(context->GetWindowHeight()) / 2.0F;

    this->m_Transform.scale = {kUiScale, kUiScale};

    const glm::vec2 backgroundSize = this->GetScaledSize();
    const glm::vec2 topLeft = {
        -halfWindowWidth + kMarginX,
        halfWindowHeight - kMarginY
    };

    this->m_Transform.translation = {
        topLeft.x + backgroundSize.x / 2.0F,
        topLeft.y - backgroundSize.y / 2.0F
    };

    this->m_HealthTextNode->m_Transform.translation = {
        topLeft.x + kHealthTextOffset.x * kUiScale,
        topLeft.y + kHealthTextOffset.y * kUiScale
    };
    this->m_ShieldTextNode->m_Transform.translation = {
        topLeft.x + kShieldTextOffset.x * kUiScale,
        topLeft.y + kShieldTextOffset.y * kUiScale
    };
    this->m_AmmoTextNode->m_Transform.translation = {
        topLeft.x + kAmmoTextOffset.x * kUiScale,
        topLeft.y + kAmmoTextOffset.y * kUiScale
    };
}

void PlayUI::SyncHudState() {
    const PlayerHudState state = this->m_HudStateProvider != nullptr ?
        this->m_HudStateProvider() :
        BuildEmptyHudState();

    if (!this->IsHudStateDirty(state)) {
        return;
    }

    this->m_HealthText->SetText(FormatValueText(state.hp, state.maxHp));
    this->m_ShieldText->SetText(FormatValueText(state.shield, state.maxShield));
    this->m_AmmoText->SetText(
        "Ammo " + FormatValueText(state.ammo, state.maxAmmo)
    );

    this->m_LastHudState = state;
    this->m_HasLastHudState = true;
}

bool PlayUI::IsHudStateDirty(const PlayerHudState &state) const {
    if (!this->m_HasLastHudState) {
        return true;
    }

    return this->m_LastHudState.hp != state.hp ||
           this->m_LastHudState.maxHp != state.maxHp ||
           this->m_LastHudState.shield != state.shield ||
           this->m_LastHudState.maxShield != state.maxShield ||
           this->m_LastHudState.ammo != state.ammo ||
           this->m_LastHudState.maxAmmo != state.maxAmmo;
}
