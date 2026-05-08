#include "Component/Prop/Prop.hpp"

Prop::Prop() {};

void Prop::Initialize(MapSystem *mapSystem) {
    (void)mapSystem;
}

bool Prop::IsDestroyRequested() const {
    return this->m_DestroyRequested;
}

void Prop::RequestDestroy() {
    this->m_DestroyRequested = true;
}

Util::Transform Prop::GetObjectTransform() const {
    return this->GetTransform();
}
