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
    RandomChoose() {
        std::random_device rand;
        
        m_Engine = std::mt19937_64(rand());
        m_Dist = std::uniform_real_distribution<float>(0.0, 1.0);
    }

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

    /// Get a random integer value in [0,  end]
    int GetInteger(int end);
    /// Get a random integer value in [start, end]
    int GetInteger(int start, int end);

    /// Get a random float value in [0, 1]
    float GetFloat();
    /// Get a random float value in [0, end]
    float GetFloat(float end);
    /// Get a random float value in [start, end]
    float GetFloat(float start, float end);

    // Get a random bool 
    bool GetBool();

    template <typename T> T ChooseFromVector(std::vector<T>& vector);

protected:
    unsigned int long long m_Seed;
    std::mt19937_64 m_Engine;
    std::uniform_real_distribution<float> m_Dist;
};

#endif