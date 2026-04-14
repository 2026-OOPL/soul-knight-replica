#ifndef PORTAL_HPP
#define PORTAL_HPP

#include "Component/Prop/Prop.hpp"
#include "Util/Image.hpp"
#include <glm/ext/vector_float2.hpp>
#include <memory>

class Portal : public Prop {
public:
    Portal(
        glm::vec2 cooridinate
    );

    void Update() override;

private:
    std::shared_ptr<Util::Image> m_PortalImage;
};

#endif 