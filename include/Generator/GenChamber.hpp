#ifndef GEN_CHAMBER_HPP
#define GEN_CHAMBER_HPP

#include <memory>
#include <vector>
#include <functional>

#include <glm/vec2.hpp>

class MapBlueprint;
class RandomChoose;

class GenChamber {
public:

    GenChamber(
        // The area limiter which tells generator whether this cooridinate could be use
        std::function<bool(glm::ivec2)> limiter,

        std::shared_ptr<MapBlueprint> blueprint,
        std::shared_ptr<RandomChoose> random
    );

    GenChamber(
        std::shared_ptr<MapBlueprint> blueprint,
        std::shared_ptr<RandomChoose> random
    );

    ~GenChamber() = default;
    
    virtual void Generate() = 0;
    virtual std::vector<glm::ivec2> GetAvailableCooridinate() = 0;
    
protected:
    bool isCooridinateInBound(glm::ivec2 position);

    std::function<bool(glm::ivec2)> m_Limiter;

    std::shared_ptr<MapBlueprint> m_Blueprint;
    std::shared_ptr<RandomChoose> m_RandomChoose;
};

#endif