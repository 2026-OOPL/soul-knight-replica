#ifndef SCENE_HPP
#define SCENE_HPP

#include <Util/Renderer.hpp>
#include <memory>

class Scene {
public:
    Scene(Util::Renderer* m_Root) {
        this->m_Root = m_Root;
    };

    virtual ~Scene() = default;

    virtual void Initialize() = 0;
    virtual void Dispose() = 0;

    virtual void Update() = 0;

    virtual std::unique_ptr<Scene> GetRedirection();

protected:
    Util::Renderer* m_Root;
};

#endif