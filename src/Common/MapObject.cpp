#include "Common/MapObject.hpp"
#include "Util/Transform.hpp"

glm::vec2 MapObject::GetObjectScale() const {
    return this->GetObjectTransform().scale;
}

glm::vec2 MapObject::GetObjectTranslation() const {
    return this->GetObjectTransform().translation;
}

float MapObject::GetObjectRotation() const {
    return this->GetObjectTransform().rotation;
}

void MapObject::SetAbsoluteScale(glm::vec2 scale) {
    this->m_AbsoluteTransform.scale = scale;
}

void MapObject::SetAbsoluteTranslation(glm::vec2 transform) {
    this->m_AbsoluteTransform.translation = transform;
}

void MapObject::SetAbsoluteRotation(float degree) {
    this->m_AbsoluteTransform.rotation = degree;
}

glm::vec2 MapObject::GetAbsoluteTranslation() const {
    return this->GetAbsoluteTransform().translation;
}

glm::vec2 MapObject::GetAbsoluteScale() const {
    return this->GetAbsoluteTransform().scale;
}

float MapObject::GetAbsoluteRotation() const {
    return this->GetAbsoluteTransform().rotation;
}

Util::Transform MapObject::GetAbsoluteTransform() const {
    return this->m_AbsoluteTransform;
}

Util::Transform MapObject::GetRenderTransform() const {
    return this->GetAbsoluteTransform();
}
