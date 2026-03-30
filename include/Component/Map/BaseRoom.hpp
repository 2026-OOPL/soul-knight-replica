#ifndef BASE_ROOM_HPP
#define BASE_ROOM_HPP

#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/IStateful.hpp"
#include "Component/Map/Door.hpp"
#include "Component/Map/RectMapArea.hpp"

class BaseRoom : public RectMapArea, public IStateful {
public:
    BaseRoom(
        const glm::vec2 &absolutePosition,
        RoomType roomType,
        RoomPurpose purpose,
        const DoorConfig &doorConfig,
        const WallConfig &wallConfig
    );

    ~BaseRoom() override = default;

    void Update() override;

    bool IsPlayerInside(const glm::vec2 &playerPos) const;

    const std::vector<Collision::AxisAlignedBox> &GetStaticColliders() const;
    std::vector<Collision::AxisAlignedBox> GetDynamicColliders(
        const Collision::AxisAlignedBox *ignoreOverlapBox = nullptr
    ) const;

    const std::vector<std::shared_ptr<Door>> &GetDoors() const;
    const std::vector<std::shared_ptr<Character>> &GetMobs() const;

    void AddMob(const std::shared_ptr<Character> &mob);

    virtual void OpenAllDoors();
    virtual void CloseAllDoors();
    virtual void OnPlayerEnter();
    virtual void OnPlayerLeave();

    glm::vec2 GetRoomSize() const;
    RoomType GetRoomType() const;
    RoomPurpose GetPurpose() const;
    bool HasPassageOnSide(DoorSide side) const;

    static WallConfig BuildWallConfigFromDoorConfig(
        const DoorConfig &doorConfig,
        float wallThickness
    );

protected:
    static glm::vec2 ResolveRoomSize(RoomType roomType);
    static std::string ResolveRoomSprite(RoomType roomType);

private:
    struct DoorBuildInfo {
        DoorSide side = DoorSide::Bottom;
        glm::vec2 colliderSize = {0.0F, 0.0F};
        glm::vec2 renderSize = {0.0F, 0.0F};
        float openingOffset = 0.0F;
        glm::vec2 positionOffset = {0.0F, 0.0F};
        bool startsOpen = true;
        Door::Visuals visuals;
    };

    static Door::Visuals BuildHorizontalDoorVisuals();
    static Door::Visuals BuildVerticalDoorVisuals();

    glm::vec2 BuildDoorPosition(const DoorBuildInfo &doorInfo) const;
    const DoorSideConfig &GetDoorSideConfig(DoorSide side) const;
    const WallSideConfig &GetWallSideConfig(DoorSide side) const;
    void BuildDoors();

    RoomType m_RoomType = RoomType::ROOM_13_13;
    RoomPurpose m_Purpose = RoomPurpose::FIGHTING;
    DoorConfig m_DoorConfig;
    std::vector<std::shared_ptr<Door>> m_Doors;
    std::vector<std::shared_ptr<Character>> m_Mobs;
};

#endif
