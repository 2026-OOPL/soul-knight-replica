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
#include "Component/Mobs/Mob.hpp"

class MapSystem;

class BaseRoom : public RectMapArea,
                 public IStateful,
                 public std::enable_shared_from_this<BaseRoom> {
public:
    struct PassageSocket {
        DoorSide side = DoorSide::Bottom;
        glm::vec2 worldCenter = {0.0F, 0.0F};
        float openingSize = 0.0F;
        float wallThickness = 0.0F;
    };

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
    std::vector<Collision::CollisionPrimitive> CollectBlockingPrimitives(
        const Collision::AxisAlignedBox *ignoreOverlapBox = nullptr
    ) const override;

    const std::vector<std::shared_ptr<Door>> &GetDoors() const;
    const std::vector<std::shared_ptr<Mob>> &GetMobs() const;

    void AddMob(const std::shared_ptr<Mob> &mob);

    virtual void OpenAllDoors();
    virtual void CloseAllDoors();
    virtual void OnPlayerEnter();
    virtual void OnPlayerLeave();

    RoomType GetRoomType() const;
    RoomPurpose GetPurpose() const;
    bool HasPassageOnSide(DoorSide side) const;
    PassageSocket GetPassageSocket(DoorSide side) const;

    static WallConfig BuildWallConfigFromDoorConfig(
        const DoorConfig &doorConfig,
        float wallThickness,
        RoomType roomType,
        RoomPurpose purpose
    );

    virtual void Initialize(MapSystem* MapSystem);

protected:
    static glm::vec2 ResolveRoomSize(RoomType roomType);
    static std::string ResolveRoomSprite(RoomType roomType);

private:
    struct DoorBuildInfo {
        DoorSide side = DoorSide::Bottom;
        glm::vec2 colliderSize = {0.0F, 0.0F};
        glm::vec2 renderSize = {0.0F, 0.0F};
        bool startsOpen = true;
        Door::Visuals visuals;
    };

    static Door::Visuals BuildHorizontalDoorVisuals();
    static Door::Visuals BuildVerticalDoorVisuals();

    const DoorSideConfig &GetDoorSideConfig(DoorSide side) const;
    const WallSideConfig &GetWallSideConfig(DoorSide side) const;
    void BuildDoors();

    RoomType m_RoomType = RoomType::ROOM_13_13;
    RoomPurpose m_Purpose = RoomPurpose::FIGHTING;
    DoorConfig m_DoorConfig;
    std::vector<std::shared_ptr<Door>> m_Doors;
    std::vector<std::shared_ptr<Mob>> m_Mobs;
};

#endif
