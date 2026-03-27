#ifndef GEN_REWARD_CHAMBER_HPP
#define GEN_REWARD_CHAMBER_HPP

#include <vector>
#include <memory>

#include <glm/vec2.hpp>

#include "GenChamber.hpp"

class GenRewardChamber : public GenChamber {
public:
    GenRewardChamber(
        // The area limiter which tells generator whether this cooridinate could be use
        std::function<bool(glm::ivec2)> limiter,

        // The chamber count limiter
        int maxChamberCount,
        int minChamberCount,

        std::shared_ptr<MapBlueprint> blueprint,
        std::shared_ptr<RandomChoose> random
    );

    ~GenRewardChamber() = default;

    void Generate() override;
    std::vector<glm::ivec2> GetAvailableCooridinate() override;

protected:
    void CreateRoom(glm::ivec2 cooridinate);

    int m_MinChamberCount;
    int m_MaxChamberCount;
};

#endif