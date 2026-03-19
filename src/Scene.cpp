#include <memory>

#include "Scene.hpp"
#include "Component/IStateful.hpp"

std::shared_ptr<Scene> Scene::GetRedirection() { 
    return nullptr;
}

void Scene::Update() {
    for (auto& i : m_Children) {
        std::shared_ptr<IStateful> stateful = std::dynamic_pointer_cast<IStateful>(i);

        if (stateful) {
            stateful->Update();
        }
    }
}
