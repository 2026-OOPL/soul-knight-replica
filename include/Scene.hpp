#ifndef SCENE_HPP
#define SCENE_HPP

#include "Util/GameObject.hpp"
#include <Util/Renderer.hpp>
#include <memory>

class Scene : public Util::GameObject {
public:
    Scene() = default;

    virtual ~Scene() = default;

    virtual void Update();

    virtual std::shared_ptr<Scene> GetRedirection();
};

#endif
