
#include "Core/Context.hpp"
#include "Util/GameObject.hpp"

class MapPiece : public Util::GameObject {
public:
    MapPiece(glm::vec2 cooridinate) {
        this->cooridinate - cooridinate;
    };

    glm::vec2 cooridinate;

    void ToggleVisibilityByCooridinate(glm::vec2 cooridinate);
};