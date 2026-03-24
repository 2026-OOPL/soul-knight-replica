#include <glm/vec2.hpp>
#include <memory>
#include <stdexcept>

#include "Generator/RandomChoose.hpp"
#include "Common/EnumMask.hpp"
#include "Common/Enums.hpp"

template <typename T>
T RandomChoose::GetEnum() {
    int size = sizeof(Direction);
    int index = this->GetInteger(0, size-1);

    return static_cast<T>(index - 1);
}

template <typename T>
T RandomChoose::GetEnum(std::shared_ptr<EnumMask<T>> mask) {
    if (mask == nullptr) {
        return this->GetEnum<T>();
    }

    const int size = sizeof(Direction);

    if (mask->isAllMasked()) {
        // Detect if mask contains every possibility
        // Take a Enum which has 4 elements, then the forbidden mask is 1111.
        throw std::runtime_error("Every element is masked, there are no possible output");
    }

    T result;

    do {
        result = static_cast<T>(this->GetInteger(0, size-1));
    } while (mask->isMasked(result));

    return result;
}

int RandomChoose::GetInteger(int start, int end) {
    float seed = m_Dist(m_Engine);
    
    return (int) (seed * (end-start)) + start;  
};

Direction RandomChoose::GetDirection() {
    return this->GetEnum<Direction>();
}

Direction RandomChoose::GetDirection(std::shared_ptr<EnumMask<Direction>> mask) {
    return this->GetEnum<Direction>(mask);
}

RoomType RandomChoose::GetRoomType() {
    return this->GetEnum<RoomType>();
}