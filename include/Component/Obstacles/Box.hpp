#include "Util/GameObject.hpp"

#include "Common/MapObject.hpp"
#include "Component/Collision/ICollidable.hpp"
#include "Component/IStateful.hpp"

class Box : public MapObject,
            public IStateful,
            public Util::GameObject,
            public ICollidable {
public:
    

};