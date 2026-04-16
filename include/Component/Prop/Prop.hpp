#ifndef PROP_HPP
#define PROP_HPP

#include "Common/MapObject.hpp"
#include "Component/IStateful.hpp"
#include "Core/Drawable.hpp"
#include "Util/GameObject.hpp"
#include <memory>

class Prop : public MapObject,
             public IStateful,
             public Util::GameObject {
public:
    Prop();

    bool IsDebugVisible() {
        return false;
    }

    std::shared_ptr<Core::Drawable> GetDebugDrawable() {
        return this->m_Drawable;
    }


    virtual Util::Transform GetObjectTransform() const override;
};

#endif