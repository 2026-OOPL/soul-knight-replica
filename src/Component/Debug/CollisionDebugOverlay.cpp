#include "Component/Debug/CollisionDebugOverlay.hpp"

#include <algorithm>
#include <cmath>

#include "Component/Debug/SolidColorRectDrawable.hpp"

namespace {

constexpr float kImageZIndex = 90.0F;
constexpr float kFillZIndex = 91.0F;
constexpr float kBorderZIndex = 92.0F;
constexpr float kLabelZIndex = 93.0F;
constexpr float kBorderThickness = 2.0F;
constexpr float kLabelOffsetY = 10.0F;
constexpr float kMinVisibleExtent = 1.0F;

const Util::Color kLabelColor = Util::Color(255, 255, 255, 255);
const std::string kFontPath = RESOURCE_DIR "/Font/Cubic-Font/Cubic_11.ttf";
constexpr int kLabelFontSize = 10;

Util::Color BuildFillColor(const Util::Color &borderColor) {
    return Util::Color(
        static_cast<Uint8>(borderColor.r),
        static_cast<Uint8>(borderColor.g),
        static_cast<Uint8>(borderColor.b),
        48
    );
}

} // namespace

CollisionDebugOverlay::CollisionDebugOverlay()
    : GameObject(nullptr, kFillZIndex) {
}

void CollisionDebugOverlay::SetEnabled(bool enabled) {
    this->m_Enabled = enabled;

    if (!this->m_Enabled) {
        this->HideUnusedSpriteVisuals(0);
        this->HideUnusedVisuals(0);
    }
}

void CollisionDebugOverlay::Sync(const CollisionDebugSnapshot &snapshot) {
    if (!this->m_Enabled) {
        this->HideUnusedSpriteVisuals(0);
        this->HideUnusedVisuals(0);
        return;
    }

    this->EnsureSpriteVisualCount(snapshot.visualEntries.size());
    for (std::size_t index = 0; index < snapshot.visualEntries.size(); ++index) {
        this->ApplySpriteVisual(this->m_SpriteVisuals[index], snapshot.visualEntries[index]);
    }
    this->HideUnusedSpriteVisuals(snapshot.visualEntries.size());

    this->EnsureVisualCount(snapshot.entries.size());

    for (std::size_t index = 0; index < snapshot.entries.size(); ++index) {
        this->ApplyEntryVisual(
            this->m_EntryVisuals[index],
            snapshot.entries[index],
            snapshot.cameraState
        );
    }

    this->HideUnusedVisuals(snapshot.entries.size());
}

void CollisionDebugOverlay::EnsureSpriteVisualCount(std::size_t count) {
    while (this->m_SpriteVisuals.size() < count) {
        SpriteVisual visual;
        visual.imageNode = std::make_shared<Util::GameObject>(nullptr, kImageZIndex);
        this->AddChild(visual.imageNode);
        this->HideSpriteVisual(visual);
        this->m_SpriteVisuals.push_back(visual);
    }
}

void CollisionDebugOverlay::HideSpriteVisual(SpriteVisual &visual) {
    visual.imageNode->SetVisible(false);
    visual.imageNode->SetDrawable(nullptr);
}

void CollisionDebugOverlay::HideUnusedSpriteVisuals(std::size_t usedCount) {
    for (std::size_t index = usedCount; index < this->m_SpriteVisuals.size(); ++index) {
        this->HideSpriteVisual(this->m_SpriteVisuals[index]);
    }
}

void CollisionDebugOverlay::ApplySpriteVisual(
    SpriteVisual &visual,
    const CollisionDebugVisualEntry &entry
) {
    if (entry.drawable == nullptr) {
        this->HideSpriteVisual(visual);
        return;
    }

    visual.imageNode->SetDrawable(entry.drawable);
    visual.imageNode->SetVisible(true);
    visual.imageNode->m_Transform.translation = entry.screenTranslation;
    visual.imageNode->m_Transform.scale = entry.screenScale;
    visual.imageNode->m_Transform.rotation = entry.screenRotation;
}

void CollisionDebugOverlay::EnsureVisualCount(std::size_t count) {
    while (this->m_EntryVisuals.size() < count) {
        EntryVisual visual;
        visual.fillDrawable = std::make_shared<SolidColorRectDrawable>(
            Util::Color(255, 255, 255, 48),
            false
        );
        visual.borderDrawable = std::make_shared<SolidColorRectDrawable>(
            Util::Color(255, 255, 255, 220),
            false
        );
        visual.labelDrawable = std::make_shared<Util::Text>(
            kFontPath,
            kLabelFontSize,
            "Debug",
            kLabelColor,
            false
        );

        visual.fillNode = std::make_shared<Util::GameObject>(visual.fillDrawable, kFillZIndex);
        visual.topBorderNode = std::make_shared<Util::GameObject>(
            visual.borderDrawable,
            kBorderZIndex
        );
        visual.bottomBorderNode = std::make_shared<Util::GameObject>(
            visual.borderDrawable,
            kBorderZIndex
        );
        visual.leftBorderNode = std::make_shared<Util::GameObject>(
            visual.borderDrawable,
            kBorderZIndex
        );
        visual.rightBorderNode = std::make_shared<Util::GameObject>(
            visual.borderDrawable,
            kBorderZIndex
        );
        visual.labelNode = std::make_shared<Util::GameObject>(visual.labelDrawable, kLabelZIndex);

        this->AddChild(visual.fillNode);
        this->AddChild(visual.topBorderNode);
        this->AddChild(visual.bottomBorderNode);
        this->AddChild(visual.leftBorderNode);
        this->AddChild(visual.rightBorderNode);
        this->AddChild(visual.labelNode);

        this->HideVisual(visual);
        this->m_EntryVisuals.push_back(visual);
    }
}

void CollisionDebugOverlay::HideVisual(EntryVisual &visual) {
    visual.fillNode->SetVisible(false);
    visual.topBorderNode->SetVisible(false);
    visual.bottomBorderNode->SetVisible(false);
    visual.leftBorderNode->SetVisible(false);
    visual.rightBorderNode->SetVisible(false);
    visual.labelNode->SetVisible(false);
}

void CollisionDebugOverlay::HideUnusedVisuals(std::size_t usedCount) {
    for (std::size_t index = usedCount; index < this->m_EntryVisuals.size(); ++index) {
        this->HideVisual(this->m_EntryVisuals[index]);
    }
}

void CollisionDebugOverlay::ApplyEntryVisual(
    EntryVisual &visual,
    const CollisionDebugEntry &entry,
    const CollisionDebugCameraState &cameraState
) {
    const glm::vec2 screenCenter = WorldToScreen(entry.worldCenter, cameraState);
    const glm::vec2 screenSize = {
        std::max(kMinVisibleExtent, entry.worldSize.x * cameraState.scale.x),
        std::max(kMinVisibleExtent, entry.worldSize.y * cameraState.scale.y)
    };
    const float screenRotation = entry.worldRotation - cameraState.rotation;
    const glm::vec2 rightAxis = {
        std::cos(screenRotation),
        std::sin(screenRotation)
    };
    const glm::vec2 upAxis = {
        -std::sin(screenRotation),
        std::cos(screenRotation)
    };
    const Util::Color fillColor = BuildFillColor(entry.color);

    visual.fillDrawable->SetColor(fillColor);
    visual.borderDrawable->SetColor(entry.color);
    visual.labelDrawable->SetText(entry.label.empty() ? "Debug" : entry.label);
    visual.labelDrawable->SetColor(entry.color);

    visual.fillNode->SetVisible(true);
    visual.topBorderNode->SetVisible(true);
    visual.bottomBorderNode->SetVisible(true);
    visual.leftBorderNode->SetVisible(true);
    visual.rightBorderNode->SetVisible(true);
    visual.labelNode->SetVisible(true);

    visual.fillNode->m_Transform.translation = screenCenter;
    visual.fillNode->m_Transform.scale = screenSize;
    visual.fillNode->m_Transform.rotation = screenRotation;

    visual.topBorderNode->m_Transform.translation =
        screenCenter + upAxis * (screenSize.y / 2.0F - kBorderThickness / 2.0F);
    visual.topBorderNode->m_Transform.scale = {
        screenSize.x + kBorderThickness * 2.0F,
        kBorderThickness
    };
    visual.topBorderNode->m_Transform.rotation = screenRotation;

    visual.bottomBorderNode->m_Transform.translation =
        screenCenter - upAxis * (screenSize.y / 2.0F - kBorderThickness / 2.0F);
    visual.bottomBorderNode->m_Transform.scale = {
        screenSize.x + kBorderThickness * 2.0F,
        kBorderThickness
    };
    visual.bottomBorderNode->m_Transform.rotation = screenRotation;

    visual.leftBorderNode->m_Transform.translation =
        screenCenter - rightAxis * (screenSize.x / 2.0F - kBorderThickness / 2.0F);
    visual.leftBorderNode->m_Transform.scale = {
        kBorderThickness,
        screenSize.y
    };
    visual.leftBorderNode->m_Transform.rotation = screenRotation;

    visual.rightBorderNode->m_Transform.translation =
        screenCenter + rightAxis * (screenSize.x / 2.0F - kBorderThickness / 2.0F);
    visual.rightBorderNode->m_Transform.scale = {
        kBorderThickness,
        screenSize.y
    };
    visual.rightBorderNode->m_Transform.rotation = screenRotation;

    visual.labelNode->m_Transform.translation =
        screenCenter + upAxis * (screenSize.y / 2.0F + kLabelOffsetY);
    visual.labelNode->m_Transform.scale = {1.0F, 1.0F};
    visual.labelNode->m_Transform.rotation = 0.0F;
}

glm::vec2 CollisionDebugOverlay::WorldToScreen(
    const glm::vec2 &worldPoint,
    const CollisionDebugCameraState &cameraState
) {
    const glm::vec2 worldDelta = worldPoint - cameraState.coordinate;
    const float cosTheta = std::cos(-cameraState.rotation);
    const float sinTheta = std::sin(-cameraState.rotation);

    const glm::vec2 rotatedTranslation = {
        worldDelta.x * cosTheta - worldDelta.y * sinTheta,
        worldDelta.x * sinTheta + worldDelta.y * cosTheta
    };

    return {
        rotatedTranslation.x * cameraState.scale.x,
        rotatedTranslation.y * cameraState.scale.y
    };
}
