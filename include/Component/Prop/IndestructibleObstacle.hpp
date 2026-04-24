#ifndef INDESTRUCTIBLE_OBSTACLE_HPP
#define INDESTRUCTIBLE_OBSTACLE_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Prop/BlockingProp.hpp"

namespace Core {
class Drawable;
}

class BaseRoom;

class IndestructibleObstacle : public BlockingProp {
public:
    struct Config {
        std::shared_ptr<BaseRoom> owningRoom = nullptr;
        glm::vec2 blockingSize = {48.0F, 32.0F};
        glm::vec2 blockingOffset = {0.0F, -6.0F};
        glm::vec2 renderSize = {64.0F, 48.0F};
        int zIndex = 3;
        std::shared_ptr<Core::Drawable> visual = nullptr;
    };

    IndestructibleObstacle(const glm::vec2 &coordinate, Config config);
    ~IndestructibleObstacle() override = default;

    void Update() override;
    bool IsDebugVisible() const override;

private:
    static glm::vec2 SafeScaleForSize(
        const std::shared_ptr<Core::Drawable> &drawable,
        const glm::vec2 &renderSize
    );
};

#endif
