#include <algorithm>
#include <stdexcept>
#include <utility>

#include "Util/Animation.hpp"
#include "Util/Time.hpp"

#include "Component/Map/Door.hpp"

namespace {

constexpr float kDoorOpenDelayMs = 200.0F; // Tune this value to change the door-open delay in ms.

} // namespace

Door::Door(
    glm::vec2 cooridinate,
    DoorSide side,
    glm::vec2 colliderSize,
    glm::vec2 renderSize,
    Visuals visuals,
    bool isOpen
)
    : MapPiece(
          cooridinate,
          isOpen ? visuals.openIdle : visuals.closedIdle,
          !isOpen
      ),
      m_Visuals(std::move(visuals)),
      m_Side(side) {
    if (this->m_Visuals.openIdle == nullptr &&
        this->m_Visuals.closedIdle == nullptr &&
        this->m_Visuals.opening == nullptr &&
        this->m_Visuals.closing == nullptr) {
        throw std::invalid_argument("Door must provide at least one visual.");
    }

    this->SetColliderSize(colliderSize);
    this->SetZIndex(4.0F);

    this->m_RenderSize = renderSize;
    this->m_BaseRotation = 0.0F;

    if (this->m_Visuals.opening != nullptr) {
        this->m_Visuals.opening->SetLooping(false);
    }
    if (this->m_Visuals.closing != nullptr) {
        this->m_Visuals.closing->SetLooping(false);
    }

    this->EnterState(isOpen ? State::Open : State::Closed);
}

void Door::Update() {
    if (this->m_State == State::OpenDelay) {
        this->m_OpenDelayRemainingMs =
            std::max(0.0F, this->m_OpenDelayRemainingMs - Util::Time::GetDeltaTimeMs());

        if (this->m_OpenDelayRemainingMs <= 0.0F) {
            if (this->m_Visuals.opening != nullptr) {
                this->EnterState(State::Opening);
            } else {
                this->EnterState(State::Open);
            }
        }
    }

    if (this->m_State == State::Opening &&
        this->m_Visuals.opening != nullptr &&
        this->m_Visuals.opening->GetState() == Util::Animation::State::ENDED) {
        this->EnterState(State::Open);
    }

    if (this->m_State == State::Closing &&
        this->m_Visuals.closing != nullptr &&
        this->m_Visuals.closing->GetState() == Util::Animation::State::ENDED) {
        this->EnterState(State::Closed);
    }
}

glm::vec2 Door::GetAbsoluteScale() const {
    if (this->m_Drawable == nullptr) {
        return {0,0 };
    }

    return this->m_Drawable->GetSize() * m_AbsoluteTransform.scale;
}

void Door::Open() {
    if (this->m_State == State::Open ||
        this->m_State == State::OpenDelay ||
        this->m_State == State::Opening) {
        return;
    }

    if (kDoorOpenDelayMs > 0.0F) {
        this->EnterState(State::OpenDelay);
    } else {
        if (this->m_Visuals.opening != nullptr) {
            this->EnterState(State::Opening);
        } else {
            this->EnterState(State::Open);
        }
    }
}

void Door::Close() {
    if (this->m_State == State::Closed || this->m_State == State::Closing) {
        return;
    }

    if (this->m_Visuals.closing != nullptr) {
        this->EnterState(State::Closing);
    } else {
        this->EnterState(State::Closed);
    }
}

void Door::Toggle() {
    if (this->m_State == State::Open ||
        this->m_State == State::OpenDelay ||
        this->m_State == State::Opening) {
        this->Close();
    } else {
        this->Open();
    }
}

bool Door::IsOpen() const {
    return this->m_State == State::Open;
}

Door::State Door::GetState() const {
    return this->m_State;
}

DoorSide Door::GetSide() const {
    return this->m_Side;
}

void Door::SetColliderOffset(const glm::vec2 &colliderOffset) {
    this->m_ColliderOffset = colliderOffset;
}

glm::vec2 Door::GetColliderCenter() const {
    return this->GetAbsoluteTranslation() + this->m_ColliderOffset;
}

void Door::ApplyDrawable(const std::shared_ptr<Core::Drawable> &drawable) {
    this->SetDrawable(drawable);
    this->m_Transform.rotation = this->m_BaseRotation;

    if (drawable == nullptr) {
        this->m_Transform.scale = {1.0F, 1.0F};
        return;
    }

    const glm::vec2 drawableSize = drawable->GetSize();
    this->m_Transform.scale = {
        this->m_RenderSize.x / std::max(drawableSize.x, 1.0F),
        this->m_RenderSize.y / std::max(drawableSize.y, 1.0F)
    };
}

void Door::EnterState(State state) {
    this->m_State = state;

    switch (state) {
    case State::Closed:
        this->ApplyDrawable(this->m_Visuals.closedIdle);
        break;

    case State::Opening:
        this->m_Visuals.opening->SetCurrentFrame(0);
        this->m_Visuals.opening->Play();
        this->ApplyDrawable(this->m_Visuals.opening);
        break;

    case State::Open:
        this->ApplyDrawable(this->m_Visuals.openIdle);
        break;

    case State::Closing:
        this->m_Visuals.closing->SetCurrentFrame(0);
        this->m_Visuals.closing->Play();
        this->ApplyDrawable(this->m_Visuals.closing);
        break;

    case State::OpenDelay:
        this->m_OpenDelayRemainingMs = kDoorOpenDelayMs;
        break;
    }
}
