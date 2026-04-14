#include "Component/Prop/Prop.hpp"

Prop::Prop() {};

Util::Transform Prop::GetObjectTransform() const {
    return this->GetTransform();
}