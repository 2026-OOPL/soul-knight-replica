#ifndef RANDOM_CHOOSE_HPP
#define RANDOM_CHOOSE_HPP

#include <memory>
#include <string>
#include <random>
#include <functional>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/glm.hpp>

#include "../Common/Enums.hpp"
#include "../Common/EnumMask.hpp"

class RandomChoose {
public:
    RandomChoose(std::string seed) {
        std::hash<std::string> hasher;
        this->m_Seed = hasher(seed);
        
        m_Engine = std::mt19937_64(m_Seed);
        m_Dist = std::uniform_real_distribution<float>(0.0, 1.0);
    }
        
    template <typename T> 
    T GetEnum();
    
    template <typename T> 
    T GetEnum(std::shared_ptr<EnumMask<T>> mask);

    int GetInteger(int end);
    int GetInteger(int start, int end);


    float GetFloat();
    float GetFloat(float end);
    float GetFloat(float start, float end);

    bool GetBool();

    template <typename T> T ChooseFromVector(std::vector<T>& vector);

protected:
    unsigned int long long m_Seed;
    std::mt19937_64 m_Engine;
    std::uniform_real_distribution<float> m_Dist;
};

#endif