#ifndef PLAYER_PLAYER_HPP
#define PLAYER_PLAYER_HPP

#include "Component/Character/Character.hpp"
#include "Component/ICollidable.hpp"//為了繼承ICollidable
#include "Component/IMapObject.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"//為了使用 m_Transform, SetDrawable(),GetScaledSize() 這些 GameObject 內部成員

#include "Util/Image.hpp"//為了使用 Util::Image
#include "Util/Input.hpp"//為了使用 Util::Input
#include "Util/Keycode.hpp"//為了使用 Util::Keycode

class Player : public Character, public ICollidable, public IMapObject {
public:
    /*設定角色圖片、圖片層級*/
    Player() : Character(
        std::make_shared<Util::Animation>(
            std::vector<std::string>{
                RESOURCE_DIR"/Character/character.png"
            },
            false,
            1
        )
    ) {

    }
    /*回傳角色碰撞箱大小*/
    glm::vec2 GetColliderSize() const { return m_ColliderSize; }
    /*設定角色碰撞箱大小*/
    void SetColliderSize(const glm::vec2 &colliderSize) {
        m_ColliderSize = colliderSize;
    }
    /*回傳角色中心位置*/
    glm::vec2 GetPosition() const { return m_Transform.translation; }
    void SetPosition(const glm::vec2 &position) {
        m_Transform.translation = position;
    }
    /*定義玩家中心點 = 玩家目前位置，玩家大小 = 玩家碰撞盒大小*/
    RectCollider GetCollider() const override {
        return RectCollider{m_Transform.translation, m_ColliderSize};
    }
    /*定義玩家可以卡住其他物件*/
    bool CanBlockMovement() const override { return true; }

    /*得到玩家想往哪移動的意圖*/
    glm::vec2 GetMoveIntent() const {
        glm::vec2 moveIntent(0.0F, 0.0F);

        if (Util::Input::IsKeyPressed(Util::Keycode::W)) {
            moveIntent.y += 1.0F;
        }
        if (Util::Input::IsKeyPressed(Util::Keycode::S)) {
            moveIntent.y -= 1.0F;
        }
        if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
            moveIntent.x -= 1.0F;
        }
        if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
            moveIntent.x += 1.0F;
        }

        if (moveIntent == glm::vec2(0.0F, 0.0F)) {
            return moveIntent;
        }
        /*為了移除斜線走路比直線快的bug*/
        return glm::normalize(moveIntent);
    }

private:
    glm::vec2 m_ColliderSize = {48.0F, 48.0F};//玩家的碰撞盒尺寸
};

#endif
