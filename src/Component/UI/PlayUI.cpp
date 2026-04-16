#include "Component/UI/PlayUI.hpp"

#include <algorithm>
#include <string>
#include <utility>

#include <glm/vec2.hpp>

#include "Core/Context.hpp"
#include "Util/Color.hpp"

namespace {

constexpr float kUiScale = 2.0F;
constexpr float kMarginX = 24.0F;
constexpr float kMarginY = 24.0F;
constexpr float kBackgroundZIndex = 99.0F;
constexpr float kBarZIndex = 99.5F;
constexpr float kTextZIndex = 100.0F;
constexpr int kFontSize = 14;

const std::string kPlayUiTexture = RESOURCE_DIR "/UI/Gameplay/panel.png";
const std::string kHealthBarTexture = RESOURCE_DIR "/UI/Gameplay/health_bar.png";
const std::string kShieldBarTexture = RESOURCE_DIR "/UI/Gameplay/armor_bar.png";
const std::string kAmmoBarTexture = RESOURCE_DIR "/UI/Gameplay/mana_bar.png";
const std::string kFontPath = RESOURCE_DIR "/Font/Cubic-Font/Cubic_11.ttf";

const Util::Color kHealthTextColor = Util::Color(255, 255, 255);
const Util::Color kShieldTextColor = Util::Color(255, 255, 255);
const Util::Color kAmmoTextColor = Util::Color(255, 255, 255);

constexpr glm::vec2 kHealthTextOffset = {47.0F, -10.0F};
constexpr glm::vec2 kShieldTextOffset = {47.0F, -22.0F};
constexpr glm::vec2 kAmmoTextOffset = {47.0F, -32.0F};
constexpr glm::vec2 kHealthBarOffset = {46.0F, -9.0F};
constexpr glm::vec2 kShieldBarOffset = {46.0F, -21.0F};
constexpr glm::vec2 kAmmoBarOffset = {46.0F, -32.0F};
constexpr float kHealthBarFullWidth =   58.0F;

PlayerHudState BuildEmptyHudState() {
    return {0, 0, 0, 0, 0, 0};
}

std::string FormatValueText(int current, int maximum) {
    return std::to_string(current) + "/" + std::to_string(maximum);
}

float BuildRatio(int current, int maximum) {
    if (maximum <= 0) {
        return 0.0F;
    }

    return std::clamp(
        static_cast<float>(current) / static_cast<float>(maximum),
        0.0F,
        1.0F
    );
}

void LayoutProgressBar(
    const std::shared_ptr<Util::Image> &barImage,
    const std::shared_ptr<Util::GameObject> &barNode,
    const glm::vec2 &barOffset,
    float ratio,
    float uiScale,
    float barFullWidth,
    const glm::vec2 &topLeft,
    float targetHeight
) {
    if (barImage == nullptr || barNode == nullptr) {
        return;
    }

    const glm::vec2 barCenter = {
        topLeft.x + barOffset.x * uiScale,
        topLeft.y + barOffset.y * uiScale
    };
    const float fillWidth = barFullWidth * std::clamp(ratio, 0.0F, 1.0F);
    const glm::vec2 sourceSize = barImage->GetSize();
    const float scaleY = sourceSize.y > 0.0F ? targetHeight / sourceSize.y : 1.0F;

    barNode->m_Transform.translation = {
        barCenter.x - barFullWidth / 2.0F,
        barCenter.y
    };
    barNode->m_Transform.scale = {
        fillWidth,
        scaleY
    };
}

} // namespace

PlayUI::PlayUI(HudStateProvider hudStateProvider)
    : GameObject(nullptr, kBackgroundZIndex),
      m_HudStateProvider(std::move(hudStateProvider)) {
    this->m_Background = std::make_shared<Util::Image>(kPlayUiTexture, false);
    this->SetDrawable(this->m_Background);
    this->m_Transform.scale = {kUiScale, kUiScale};

    this->m_HealthBar = std::make_shared<Util::Image>(kHealthBarTexture, false);
    this->m_HealthBarNode = std::make_shared<Util::GameObject>(
        this->m_HealthBar,
        kBarZIndex,
        glm::vec2{-0.5F, 0.0F}
    );
    this->AddChild(this->m_HealthBarNode);

    this->m_ShieldBar = std::make_shared<Util::Image>(kShieldBarTexture, false);
    this->m_ShieldBarNode = std::make_shared<Util::GameObject>(
        this->m_ShieldBar,
        kBarZIndex,
        glm::vec2{-0.5F, 0.0F}
    );
    this->AddChild(this->m_ShieldBarNode);

    this->m_AmmoBar = std::make_shared<Util::Image>(kAmmoBarTexture, false);
    this->m_AmmoBarNode = std::make_shared<Util::GameObject>(
        this->m_AmmoBar,
        kBarZIndex,
        glm::vec2{-0.5F, 0.0F}
    );
    this->AddChild(this->m_AmmoBarNode);

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
        "0/0",
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

    this->SyncHudState();
    this->UpdateLayout();
}

void PlayUI::Update() {
    this->SyncHudState();
    this->UpdateLayout();
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

    const float barFullWidth = kHealthBarFullWidth * kUiScale;
    const float barTargetHeight = this->m_HealthBar->GetSize().y * kUiScale;

    LayoutProgressBar(
        this->m_HealthBar,
        this->m_HealthBarNode,
        kHealthBarOffset,
        BuildRatio(this->m_LastHudState.hp, this->m_LastHudState.maxHp),
        kUiScale,
        barFullWidth,
        topLeft,
        barTargetHeight
    );
    LayoutProgressBar(
        this->m_ShieldBar,
        this->m_ShieldBarNode,
        kShieldBarOffset,
        BuildRatio(this->m_LastHudState.shield, this->m_LastHudState.maxShield),
        kUiScale,
        barFullWidth,
        topLeft,
        barTargetHeight
    );
    LayoutProgressBar(
        this->m_AmmoBar,
        this->m_AmmoBarNode,
        kAmmoBarOffset,
        BuildRatio(this->m_LastHudState.ammo, this->m_LastHudState.maxAmmo),
        kUiScale,
        barFullWidth,
        topLeft,
        barTargetHeight
    );

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
        FormatValueText(state.ammo, state.maxAmmo)
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
