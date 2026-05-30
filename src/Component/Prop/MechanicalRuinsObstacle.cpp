#include <memory>
#include <utility>

#include "Component/Prop/MechanicalRuinsObstacle.hpp"

#include "Util/Image.hpp"

namespace {

constexpr char kMechanicalRuinsBlockSprite[] =
    RESOURCE_DIR "/Map/obstacle/Indestructible/MechanicalRuinsBlock.png";
constexpr char kMechanicalRuinsFenceSprite[] =
    RESOURCE_DIR "/Map/obstacle/Indestructible/MechanicalRuinsFence.png";
constexpr char kMechanicalRuinsPillarSprite[] =
    RESOURCE_DIR "/Map/obstacle/Indestructible/MechanicalRuinsPillar.png";

IndestructibleObstacle::Config BuildMechanicalRuinsConfig(
    MechanicalRuinsObstacleConfig config,
    const char *spritePath,
    const glm::vec2 &blockingSize,
    const glm::vec2 &blockingOffset,
    const glm::vec2 &renderSize
) {
    IndestructibleObstacle::Config obstacleConfig;
    obstacleConfig.owningRoom = std::move(config.owningRoom);
    obstacleConfig.blockingSize = blockingSize;
    obstacleConfig.blockingOffset = blockingOffset;
    obstacleConfig.renderSize = renderSize;
    obstacleConfig.zIndex = 3;
    obstacleConfig.visual = std::make_shared<Util::Image>(spritePath, false);
    return obstacleConfig;
}

} // namespace

MechanicalRuinsBlock::MechanicalRuinsBlock(
    const glm::vec2 &coordinate,
    MechanicalRuinsObstacleConfig config
)
    : IndestructibleObstacle(
          coordinate,
          BuildMechanicalRuinsConfig(
              std::move(config),
              kMechanicalRuinsBlockSprite,
              {48.0F, 32.0F},
              {0.0F, -6.0F},
              {64.0F, 48.0F}
          )
      ) {
}

MechanicalRuinsFence::MechanicalRuinsFence(
    const glm::vec2 &coordinate,
    MechanicalRuinsObstacleConfig config
)
    : IndestructibleObstacle(
          coordinate,
          BuildMechanicalRuinsConfig(
              std::move(config),
              kMechanicalRuinsFenceSprite,
              {52.0F, 26.0F},
              {0.0F, -11.0F},
              {64.0F, 48.0F}
          )
      ) {
}

MechanicalRuinsPillar::MechanicalRuinsPillar(
    const glm::vec2 &coordinate,
    MechanicalRuinsObstacleConfig config
)
    : IndestructibleObstacle(
          coordinate,
          BuildMechanicalRuinsConfig(
              std::move(config),
              kMechanicalRuinsPillarSprite,
              {28.0F, 28.0F},
              {0.0F, -18.0F},
              {32.0F, 64.0F}
          )
      ) {
}
