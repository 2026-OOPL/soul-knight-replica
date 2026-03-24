#ifndef ENUM_MASK_HPP
#define ENUM_MASK_HPP

template <typename T>
class EnumMask {
public:
    EnumMask() {};

    bool isMasked(T e);
    bool isAllMasked();
    
    void AddMask(T e);
    void ClearMask(T e);
    void ResetMask();

protected:
    // This is a mask that indicate which of the Direction is used
    // If enum idnex n is used, then 2^n will be added to usedMask
    // So we can use bitwise and to check if the Direction appeared before
    unsigned int m_Mask = 0;
};

#endif