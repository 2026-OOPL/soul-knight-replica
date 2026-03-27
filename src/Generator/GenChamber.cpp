#include <memory>
#include <limits>
#include <stdexcept>

#include <glm/vec2.hpp>

#include "Generator/MapBlueprint.hpp"
#include "Generator/MapGenerator.hpp"
#include "Generator/GenChamber.hpp"

GenChamber::GenChamber(

    // The area limiter which tells generator whether this cooridinate could be use
    std::function<bool(glm::ivec2)> limiter,


    std::shared_ptr<MapBlueprint> blueprint,
    std::shared_ptr<RandomChoose> random
) {
    if (blueprint == nullptr) {
        throw std::runtime_error("Blueprint (which is null) is required for chamber generation");
    }

    if (random == nullptr) {
        throw std::runtime_error("Random chooser is nullptr");
    }

    this->m_Blueprint = blueprint;
    this->m_RandomChoose = random;

    this->m_Limiter = limiter;
}

GenChamber::GenChamber(
    std::shared_ptr<MapBlueprint> blueprint,
    std::shared_ptr<RandomChoose> random
) : GenChamber(
    nullptr,

    blueprint,
    random
) {};

bool GenChamber::isCooridinateInBound(glm::ivec2 cooridinate) {
    if (!this->m_Blueprint->isCooridinateInBound(cooridinate)) {
        return false;
    }
    
    if (this->m_Limiter == nullptr) {
        return true;
    }

    return this->m_Limiter(cooridinate);
}