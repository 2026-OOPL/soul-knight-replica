#include "Common/EnumMask.hpp"

template <typename T>
bool EnumMask<T>::isMasked(T e) {
    int id = static_cast<int>(e);
    return (m_Mask & (1 << id)) != 0;
}

template <typename T>
bool EnumMask<T>::isAllMasked() {
    int size = sizeof(T);
    unsigned int forbiddenMask = ((1 << size) - 1);

    if ((m_Mask & forbiddenMask) == forbiddenMask) {
        // Detect if mask contains every possibility
        // Take a Enum which has 4 elements, then the forbidden mask is 1111.
        return true;
    }

    return false;
}

template <typename T>
void EnumMask<T>::AddMask(T e) {
    int id = static_cast<int>(e);
    m_Mask = m_Mask | (1 << id);
}

template <typename T>
void EnumMask<T>::ClearMask(T e) {
    int id = static_cast<int>(e);
    m_Mask = m_Mask & ~(1 << id);
}

template <typename T>
void EnumMask<T>::ResetMask() {
    m_Mask = 0;
}