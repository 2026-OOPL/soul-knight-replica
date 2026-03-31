#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <glm/fwd.hpp>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Transform.hpp"

#include "Common/MapObject.hpp"
#include "Component/Collision/ICollidable.hpp"
#include "Component/IStateful.hpp"
#include "Component/Weapon.hpp"

class Character : public MapObject,
                  public IStateful,
                  public Util::GameObject,
                  public ICollidable {
public:
    using CollisionResolver = std::function<Collision::MovementResult(
        const ICollidable &,
        const glm::vec2 &
    )>;

    Character(
        std::shared_ptr<Util::Animation> StandAnimation,
        std::shared_ptr<Util::Animation> WalkAnimation,
        std::shared_ptr<Util::Animation> DieAnimation,
        int zIndex
    );

    Character(
        const std::vector<std::string>& StandSprite,
        const std::vector<std::string>& WalkSprite,
        const std::vector<std::string>& DieSprite,
        int zIndex
    );

    // Override for MapObject
    Util::Transform GetObjectTransform() const override; 

    std::shared_ptr<Weapon> GetWeapon();
    void SetWeapon(std::shared_ptr<Weapon> weapon);

    void Update() override;

    glm::vec2 GetCollisionOrigin() const override;
    const std::vector<Collision::CollisionBox> &GetCollisionBoxes() const override;
    void OnCollision(const Collision::CollisionSituation &situation) override;

    glm::vec2 GetColliderSize() const;
    void SetColliderSize(const glm::vec2 &colliderSize);
    Collision::CollisionFilter GetCollisionFilter() const;
    void SetCollisionFilter(const Collision::CollisionFilter &filter);
    Collision::AxisAlignedBox GetCollisionBox() const;
    Collision::AxisAlignedBox GetCollisionBoxAt(const glm::vec2 &coordinate) const;
    void SetCollisionResolver(CollisionResolver collisionResolver);

    virtual glm::vec2 GetMoveIntent() const;

protected:
    float m_Health = 10;
    
    float m_PlayerSpeed = 0.15F;

    glm::vec2 m_LastMomentum;

    std::shared_ptr<Weapon> m_Weapon;
 
    std::shared_ptr<Util::Animation> m_DieAnimation;
    std::shared_ptr<Util::Animation> m_StandAnimation;
    std::shared_ptr<Util::Animation> m_WalkAnimation;
    std::vector<Collision::CollisionBox> m_CollisionBoxes;
    CollisionResolver m_CollisionResolver = nullptr;

private:
    void SetLookDirectionByMoveIntent(glm::vec2 moveIntent);
    void SetSpriteTypeByMoveIntent(glm::vec2 moveIntent);
};

#endif
