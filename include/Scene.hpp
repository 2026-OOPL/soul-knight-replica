#ifndef SCENE_HPP
#define SCENE_HPP

#include "Component/IStateful.hpp"
#include "Util/GameObject.hpp"
#include <Util/Renderer.hpp>
#include <memory>

class Scene : public Util::GameObject, public IStateful {
public:
    Scene() = default;

    virtual ~Scene() = default;

    virtual void Update();

    virtual std::shared_ptr<Scene> GetRedirection();
};

#endif
