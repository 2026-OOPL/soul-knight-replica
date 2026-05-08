#ifndef AMMO_ORB_HPP
#define AMMO_ORB_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Prop/Prop.hpp"

class MapSystem;
class Player;

namespace Util {
class Image;
}

class AmmoOrb : public Prop {
public:
    struct Config {
        int ammoAmount = 5;
        glm::vec2 renderSize = {18.0F, 18.0F};
        float lingerDurationMs = 300.0F;
        float attractRadius = 120.0F;
        float collectRadius = 14.0F;
        float acceleration = 0.0008F;
        float maxSpeed = 0.18F;
        int zIndex = 3;
    };

    explicit AmmoOrb(const glm::vec2 &coordinate);
    AmmoOrb(const glm::vec2 &coordinate, Config config);

    void Initialize(MapSystem *mapSystem) override;
    void Update() override;

private:
    bool CanTargetPlayer(const std::shared_ptr<Player> &player) const;
    std::shared_ptr<Player> FindTargetPlayer() const;

    MapSystem *m_MapSystem = nullptr;
    std::shared_ptr<Util::Image> m_Image = nullptr;
    glm::vec2 m_RenderSize = {18.0F, 18.0F};
    int m_AmmoAmount = 5;
    float m_LingerRemainingMs = 0.0F;
    float m_AttractRadius = 120.0F;
    float m_CollectRadius = 14.0F;
    float m_Acceleration = 0.0008F;
    float m_MaxSpeed = 0.18F;
    float m_CurrentSpeed = 0.0F;
};

#endif
