#ifndef GEN_BOSS_CHAMBER_HPP
#define GEN_BOSS_CHAMBER_HPP

#include <functional>
#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Generator/GenChamber.hpp"
#include "Generator/RoomInfo.hpp"

class GenBossChamber : public GenChamber {
public:
    GenBossChamber(
        MobType mobType,
        std::function<bool(glm::ivec2)> limiter,
        std::shared_ptr<MapBlueprint> blueprint,
        std::shared_ptr<RandomChoose> random
    );

    ~GenBossChamber() = default;

    void Generate() override;
    std::vector<glm::ivec2> GetAvailableCooridinate() override;

private:
    MobType m_MobType;
};

#endif
