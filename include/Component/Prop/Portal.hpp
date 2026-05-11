#ifndef PORTAL_HPP
#define PORTAL_HPP

#include <memory>
#include <glm/vec2.hpp>

#include "Component/Prop/Prop.hpp"
#include "Util/Image.hpp"

class Portal : public Prop {
public:
    Portal(
        glm::vec2 cooridinate
    );

    void Update() override;
    void Initialize(MapSystem *mapSystem) override;

private:
    std::shared_ptr<Util::Image> m_PortalImage;
};

#endif 