#include "Component/Prop/Portal.hpp"
#include "Util/Logger.hpp"
#include <memory>

Portal::Portal(glm::vec2 cooridinate) {
    this->m_PortalImage = std::make_shared<Util::Image>(
        RESOURCE_DIR"/Map/Prop/Portal.png", 
        false
    );

    this->SetDrawable(this->m_PortalImage);
    this->SetAbsoluteTranslation(cooridinate);
    this->SetAbsoluteScale({.25, .25});

    Collision::CollisionBox box;
    box.size = glm::vec2(16, 48);
    box.filter.blocking = false;
    box.filter.trigger = true;
    box.filter.layer = Collision::CollisionLayer::Trigger;
    box.filter.mask = Collision::ToMask(Collision::CollisionLayer::Player);
    this->m_CollisionBoxes.push_back(box);
}

void Portal::Update() {
    // TODO: Implement player collision detection and callback function
}

void Portal::Initialize(MapSystem *mapSystem) {
    (void)mapSystem;

    this->SetZIndex(10);
}

glm::vec2 Portal::GetCollisionOrigin() const {
    return this->GetAbsoluteTranslation();
}

void Portal::OnCollision(const Collision::CollisionSituation &situation) {
    if (situation.otherFilter.layer == Collision::CollisionLayer::Player) {
        m_PlayerEntered = true;
    }
};

const std::vector<Collision::CollisionBox> &Portal::GetCollisionBoxes() const {
    return this->m_CollisionBoxes;
}

bool Portal::GetPlayerEntered() const {
    return this->m_PlayerEntered;
}