#include "Component/Map/Door.hpp"

#include <algorithm>
#include <cmath>

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include "Util/Color.hpp"
#include "Util/Time.hpp"

namespace {

constexpr char kDoorFont[] = RESOURCE_DIR "/Font/Cubic-Font/Cubic_11.ttf";
constexpr char kDoorGlyph[] = "========";
constexpr int kDoorFontSize = 36;
constexpr float kOpenScaleRatio = 0.15F;
constexpr float kAnimationUnitsPerMs = 0.008F;

} // namespace

Door::Door(
    glm::vec2 cooridinate,
    DoorSide side,
    glm::vec2 colliderSize,
    bool isOpen
)
    : MapPiece(
          cooridinate,
          std::make_shared<Util::Text>(
              kDoorFont,
              kDoorFontSize,
              kDoorGlyph,
              Util::Color(133, 92, 57)
          ),
          !isOpen
      ),
      m_Side(side),
      m_IsOpen(isOpen) {
    this->m_DrawableText = std::dynamic_pointer_cast<Util::Text>(this->m_Drawable);
    this->SetColliderSize(colliderSize);
    this->SetZIndex(4.0F);

    const glm::vec2 drawableSize = this->m_DrawableText->GetSize();
    const bool isVertical = side == DoorSide::Left || side == DoorSide::Right;
    const float desiredLength = isVertical ? colliderSize.y : colliderSize.x;
    const float desiredThickness = isVertical ? colliderSize.x : colliderSize.y;

    this->m_ClosedScale = {
        desiredLength / std::max(drawableSize.x, 1.0F),
        desiredThickness / std::max(drawableSize.y, 1.0F)
    };
    this->m_OpenScale = {
        this->m_ClosedScale.x,
        std::max(this->m_ClosedScale.y * kOpenScaleRatio, 0.01F)
    };
    this->m_BaseRotation = isVertical ? glm::half_pi<float>() : 0.0F;

    this->m_Transform.rotation = this->m_BaseRotation;
    this->m_Transform.scale = this->m_IsOpen ? this->m_OpenScale : this->m_ClosedScale;
}

void Door::Update() {
    const glm::vec2 targetScale =
        this->m_IsOpen ? this->m_OpenScale : this->m_ClosedScale;

    this->m_Transform.rotation = this->m_BaseRotation;
    this->m_Transform.scale = this->MoveTowards(
        this->m_Transform.scale,
        targetScale,
        kAnimationUnitsPerMs * Util::Time::GetDeltaTimeMs()
    );
}

void Door::Open() {
    this->m_IsOpen = true;
    this->SetIsWall(false);
}

void Door::Close() {
    this->m_IsOpen = false;
    this->SetIsWall(true);
}

void Door::Toggle() {
    if (this->m_IsOpen) {
        this->Close();
    } else {
        this->Open();
    }
}

bool Door::IsOpen() const {
    return this->m_IsOpen;
}

bool Door::IsClosed() const {
    return !this->m_IsOpen;
}

DoorSide Door::GetSide() const {
    return this->m_Side;
}

glm::vec2 Door::MoveTowards(
    const glm::vec2 &current,
    const glm::vec2 &target,
    float maxDistanceDelta
) const {
    glm::vec2 next = current;

    for (int index = 0; index < 2; ++index) {
        const float delta = target[index] - current[index];

        if (std::abs(delta) <= maxDistanceDelta) {
            next[index] = target[index];
        } else {
            next[index] = current[index] + glm::sign(delta) * maxDistanceDelta;
        }
    }

    return next;
}
