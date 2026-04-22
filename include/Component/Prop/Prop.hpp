#ifndef PROP_HPP
#define PROP_HPP

#include "Common/MapObject.hpp"
#include "Component/IStateful.hpp"
#include "Core/Drawable.hpp"
#include "Util/GameObject.hpp"
#include <memory>

class MapSystem;

class Prop : public MapObject,
             public IStateful,
             public Util::GameObject {
public:
    Prop();
    ~Prop() override = default;

    virtual void Initialize(MapSystem *mapSystem);
    bool IsDestroyRequested() const;
    void RequestDestroy();

    bool IsDebugVisible() {
        return false;
    }

    std::shared_ptr<Core::Drawable> GetDebugDrawable() {
        return this->m_Drawable;
    }


    virtual Util::Transform GetObjectTransform() const override;

private:
    bool m_DestroyRequested = false;
};

#endif
