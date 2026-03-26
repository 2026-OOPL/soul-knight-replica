#include <glm/vec2.hpp>
#include <memory>
#include <stdexcept>

#include "Common/Random.hpp"
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

int RandomChoose::GetInteger(int end) {
    return this->GetInteger(0, end);
}

float RandomChoose::GetFloat() {
    float seed = m_Dist(m_Engine);
    return seed;
}

float RandomChoose::GetFloat(float end) {
    return end * this->GetFloat();
}

float RandomChoose::GetFloat(float start, float end) {
    return (end - start) * this->GetFloat() + start;
}

bool RandomChoose::GetBool() {
    float seed = m_Dist(m_Engine);
    return seed > 0.5;
}

template <typename T> 
T RandomChoose::ChooseFromVector(std::vector<T>& vector) {
    int index = this->GetInteger(vector.size()-1);
    return vector[index];
}