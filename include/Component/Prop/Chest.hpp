#ifndef CHEST_HPP
#define CHEST_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Prop/BlockingProp.hpp"
#include "Component/Prop/ChestReward.hpp"

class BaseRoom;
class MapSystem;
class Player;

namespace Core {
class Drawable;
}

class Chest : public BlockingProp {
public:
    struct Visuals {
        std::shared_ptr<Core::Drawable> closedIdle = nullptr;
        std::shared_ptr<Core::Drawable> openIdle = nullptr;
    };

    struct Config {
        std::shared_ptr<BaseRoom> owningRoom = nullptr;
        glm::vec2 blockingSize = {32.0F, 32.0F};
        glm::vec2 blockingOffset = {0.0F, 0.0F};
        glm::vec2 renderSize = {32.0F, 32.0F};
        float autoOpenRange = 40.0F;
        int zIndex = 2;
        Visuals visuals;
        std::shared_ptr<IChestReward> reward = nullptr;
    };

    Chest(const glm::vec2 &coordinate, Config config);
    ~Chest() override = default;

    void Initialize(MapSystem *mapSystem) override;
    void Update() override;

    bool IsOpen() const;
    void SetReward(const std::shared_ptr<IChestReward> &reward);

private:
    void Open(Player &player);
    void ApplyCurrentVisual();
    void ApplyDrawable(const std::shared_ptr<Core::Drawable> &drawable);

    Visuals m_Visuals;
    std::shared_ptr<IChestReward> m_Reward = nullptr;
    MapSystem *m_MapSystem = nullptr;
    glm::vec2 m_RenderSize = {32.0F, 32.0F};
    float m_AutoOpenRange = 40.0F;
    bool m_IsOpen = false;
};

#endif
