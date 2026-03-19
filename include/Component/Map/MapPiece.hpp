#ifndef MAP_PIECE_HPP
#define MAP_PIECE_HPP

#include <glm/vec2.hpp>
#include <memory>
#include <string>
#include <sys/types.h>

#include "Component/ICollidable.hpp"
#include "Component/IMapObject.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

/*繼承 Util::GameObject 表示要能夠顯示在畫面上，繼承 ICollidable 表示這是有參與碰撞的地圖區塊*/
class MapPiece : public Util::GameObject, public IMapObject {
public:
    /*輸入此碰撞區塊的位置、圖片路徑、是否為牆、*/
    MapPiece(glm::vec2 cooridinate, std::string resource, bool isWall = false)
        : m_Cooridinate(cooridinate),
          m_IsWall(isWall) 
    {
        m_Image = std::make_shared<Util::Image>(resource);//建立圖片物件
        SetDrawable(m_Image);//設定m_Image為可畫的
        m_ColliderSize = GetScaledSize();//取出圖片大小乘上大小倍率
    };

    glm::vec2 GetObjectSize() override;
    glm::vec2 GetCooridinate() override;
    Util::Transform GetTransform() override;

    void SetTransformByCooridinate(glm::vec2 cooridinate);//根據視角將地圖座標轉換成相機座標

    bool IsWall() const { return m_IsWall; }//判斷是否是牆 
    void SetIsWall(bool isWall) { m_IsWall = isWall; }//可以根據物件不同狀態修改是否為牆

    glm::vec2 GetPosition() const { return m_Cooridinate; }//回傳地圖座標

    glm::vec2 GetColliderSize() const { return m_ColliderSize; }//回傳碰撞盒的寬、高
    void SetColliderSize(const glm::vec2 &colliderSize)//手動設定碰撞盒大小 
    {
        m_ColliderSize = colliderSize;
    }

private:
    glm::vec2 m_Cooridinate;//地圖座標
    std::shared_ptr<Util::Image> m_Image;//圖片路徑
    bool m_IsWall = false;//設定這塊地圖是否為牆
    glm::vec2 m_ColliderSize = {0.0F, 0.0F};//碰撞盒大小

};

#endif
