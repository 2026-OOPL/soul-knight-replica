#include <memory>

#include "Scene.hpp"
#include "Component/IStateful.hpp"

std::shared_ptr<Scene> Scene::GetRedirection() { 
    return nullptr;
}

void Scene::Update() {
    for (auto& i : m_Children) {
        this->Update(i);
    }
}

void Scene::Update(std::shared_ptr<Util::GameObject> child) {
    if (child == nullptr) {
        return;
    }
    
    std::shared_ptr<IStateful> stateful = std::dynamic_pointer_cast<IStateful>(child);

    if (stateful) {
        stateful->Update();
    }

    for (auto& i : child->GetChildren()) {
        this->Update(i);
    }
}