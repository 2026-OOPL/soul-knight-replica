#ifndef GEN_PORTAL_CHAMBER_HPP
#define GEN_PORTAL_CHAMBER_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <vector>
#include <functional>

#include "GenChamber.hpp"

enum class Direction;

enum class GeneratePolicy {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

class GenPortalChamber : public GenChamber {
public:
    GenPortalChamber(
        GeneratePolicy generatePolicy,

        // The area limiter which tells generator whether this cooridinate could be use
        std::function<bool(glm::ivec2)> limiter,
        std::shared_ptr<MapBlueprint> blueprint,
        std::shared_ptr<RandomChoose> random
    );

    ~GenPortalChamber() = default;

    void Generate() override;
    std::vector<glm::ivec2> GetAvailableCooridinate() override;

    bool SortFunction(glm::ivec2 a, glm::ivec2 b);

protected:
    GeneratePolicy m_GeneratePolicy;
};

#endif