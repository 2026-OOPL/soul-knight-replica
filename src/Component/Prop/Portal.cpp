#include "Component/Prop/Portal.hpp"
#include <memory>

Portal::Portal(glm::vec2 cooridinate) {
    this->m_PortalImage = std::make_shared<Util::Image>(
        RESOURCE_DIR"/Map/Prop/Portal.png", 
        false
    );

    this->SetDrawable(this->m_PortalImage);
    this->SetAbsoluteTranslation(cooridinate);

    this->SetAbsoluteScale({.25, .25});
}

void Portal::Update() {
    // TODO: Implement player collision detection and callback function
}