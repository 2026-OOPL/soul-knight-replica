#ifndef GEN_FIGHT_CHAMBER_HPP
#define GEN_FIGHT_CHAMBER_HPP

#include <vector>
#include <memory>
#include <functional>

#include <glm/vec2.hpp>

#include "GenChamber.hpp"

class GenChamber;
class RandomChoose;
class MapBlueprint;

class GenFightChamber : public GenChamber {
public:
    GenFightChamber(
        glm::vec2 start,

        // The area limiter which tells generator whether this cooridinate could be use
        std::function<bool(glm::vec2)> limiter,

        // The chamber count limiter
        int maxChamberCount,
        int minChamberCount,

        std::shared_ptr<MapBlueprint> blueprint,
        std::shared_ptr<RandomChoose> random
    );

    ~GenFightChamber() = default;

    void Generate() override;
    std::vector<glm::vec2> GetAvailableCooridinate() override;

protected:
    void CreateRoom(glm::vec2 position);

    glm::vec2 m_Start;

    int m_MaxChamberCount;
    int m_MinChamberCount;
};

#endif