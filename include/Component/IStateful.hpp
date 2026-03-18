#ifndef STATEFUL_COMPONENT_HPP
#define STATEFUL_COMPONENT_HPP

class IStateful {
public:
    virtual void Update() = 0;

    virtual ~IStateful() = default;
};

#endif