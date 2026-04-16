#include "Component/Debug/CollisionDebugSnapshot.hpp"

#include <unordered_set>

#include "Component/Bullet.hpp"
#include "Component/Camera/Camera.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Door.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Player/Player.hpp"
#include "Core/Drawable.hpp"

namespace {

std::string DescribeCollisionLayer(Collision::CollisionLayer layer) {
    switch (layer) {
    case Collision::CollisionLayer::World:
        return "World";
    case Collision::CollisionLayer::Player:
        return "Player";
    case Collision::CollisionLayer::Enemy:
        return "Enemy";
    case Collision::CollisionLayer::PlayerProjectile:
        return "PlayerProjectile";
    case Collision::CollisionLayer::EnemyProjectile:
        return "EnemyProjectile";
    case Collision::CollisionLayer::Prop:
        return "Prop";
    case Collision::CollisionLayer::Trigger:
        return "Trigger";
    case Collision::CollisionLayer::None:
    default:
        return "None";
    }
}

std::string DescribeCollisionBoxType(Collision::CollisionBoxType type) {
    switch (type) {
    case Collision::CollisionBoxType::Body:
        return "Body";
    case Collision::CollisionBoxType::Hitbox:
        return "Hitbox";
    case Collision::CollisionBoxType::Hurtbox:
        return "Hurtbox";
    case Collision::CollisionBoxType::Trigger:
        return "Trigger";
    default:
        return "Unknown";
    }
}

std::string DescribeCollisionOwner(const ICollidable *owner) {
    if (owner == nullptr) {
        return "World";
    }

    if (dynamic_cast<const Player *>(owner) != nullptr) {
        return "Player";
    }

    if (dynamic_cast<const Bullet *>(owner) != nullptr) {
        return "Bullet";
    }

    if (dynamic_cast<const Door *>(owner) != nullptr) {
        return "Door";
    }

    if (dynamic_cast<const Character *>(owner) != nullptr) {
        return "Character";
    }

    return "Collider";
}

Util::Color DescribeCollisionColor(const Collision::CollisionPrimitive &primitive) {
    switch (primitive.filter.layer) {
    case Collision::CollisionLayer::Player:
        return Util::Color(88, 214, 141, 220);
    case Collision::CollisionLayer::Enemy:
        return Util::Color(231, 76, 60, 220);
    case Collision::CollisionLayer::PlayerProjectile:
        return Util::Color(241, 196, 15, 220);
    case Collision::CollisionLayer::EnemyProjectile:
        return Util::Color(230, 126, 34, 220);
    case Collision::CollisionLayer::Prop:
        return Util::Color(155, 89, 182, 220);
    case Collision::CollisionLayer::Trigger:
        return Util::Color(52, 152, 219, 220);
    case Collision::CollisionLayer::World:
        return Util::Color(149, 165, 166, 220);
    case Collision::CollisionLayer::None:
    default:
        return Util::Color(236, 240, 241, 220);
    }
}

CollisionDebugEntry BuildCollisionDebugEntry(const Collision::CollisionPrimitive &primitive) {
    CollisionDebugEntry entry;
    entry.worldCenter = primitive.box.center;
    entry.worldSize = primitive.box.size;
    entry.worldRotation = 0.0F;
    entry.color = DescribeCollisionColor(primitive);
    entry.label = DescribeCollisionOwner(primitive.owner) +
                  " | " + DescribeCollisionBoxType(primitive.type) +
                  " | " + DescribeCollisionLayer(primitive.filter.layer);
    return entry;
}

CollisionDebugEntry BuildStaticCollisionDebugEntry(
    const Collision::AxisAlignedBox &box,
    const std::string &ownerLabel,
    const Util::Color &color
) {
    CollisionDebugEntry entry;
    entry.worldCenter = box.center;
    entry.worldSize = box.size;
    entry.worldRotation = 0.0F;
    entry.color = color;
    entry.label = ownerLabel + " | Body | World";
    return entry;
}

template <typename TObject>
void AppendDebugVisualEntry(
    const std::shared_ptr<TObject> &object,
    std::vector<CollisionDebugVisualEntry> &entries,
    std::unordered_set<const void *> &seenOwners
) {
    if (object == nullptr || !seenOwners.insert(object.get()).second || !object->IsDebugVisible()) {
        return;
    }

    const std::shared_ptr<Core::Drawable> drawable = object->GetDebugDrawable();
    if (drawable == nullptr) {
        return;
    }

    CollisionDebugVisualEntry entry;
    entry.drawable = drawable;
    entry.screenTranslation = object->GetTransform().translation;
    entry.screenScale = object->GetTransform().scale;
    entry.screenRotation = object->GetTransform().rotation;
    entries.push_back(entry);
}

template <typename TObject>
void AppendDynamicCollisionEntries(
    const std::shared_ptr<TObject> &object,
    std::vector<CollisionDebugEntry> &entries
) {
    if (object == nullptr) {
        return;
    }

    const std::vector<Collision::CollisionPrimitive> primitives =
        Collision::CollisionSystem::BuildCollisionPrimitives(*object);
    for (const auto &primitive : primitives) {
        entries.push_back(BuildCollisionDebugEntry(primitive));
    }
}

void AppendStaticCollisionEntries(
    const std::vector<Collision::AxisAlignedBox> &boxes,
    const std::string &ownerLabel,
    const Util::Color &color,
    std::vector<CollisionDebugEntry> &entries
) {
    for (const auto &box : boxes) {
        entries.push_back(BuildStaticCollisionDebugEntry(box, ownerLabel, color));
    }
}

void AppendRoomVisuals(
    const std::shared_ptr<BaseRoom> &room,
    const std::vector<std::shared_ptr<Gangway>> &gangways,
    std::vector<CollisionDebugVisualEntry> &entries,
    std::unordered_set<const void *> &seenOwners
) {
    if (room == nullptr) {
        return;
    }

    AppendDebugVisualEntry(room, entries, seenOwners);
    for (const auto &door : room->GetDoors()) {
        AppendDebugVisualEntry(door, entries, seenOwners);
    }

    for (const auto &gangway : gangways) {
        if (gangway == nullptr || !gangway->ConnectsRoom(room)) {
            continue;
        }

        AppendDebugVisualEntry(gangway, entries, seenOwners);
    }
}

void AppendRoomColliders(
    const std::shared_ptr<BaseRoom> &room,
    const std::vector<std::shared_ptr<Gangway>> &gangways,
    std::vector<CollisionDebugEntry> &entries,
    std::unordered_set<const Gangway *> &collectedGangways
) {
    if (room == nullptr) {
        return;
    }

    AppendStaticCollisionEntries(
        room->GetStaticColliders(),
        "Room",
        Util::Color(149, 165, 166, 220),
        entries
    );

    for (const auto &door : room->GetDoors()) {
        if (door == nullptr) {
            continue;
        }

        const std::vector<Collision::CollisionPrimitive> primitives =
            door->CollectBlockingPrimitives();
        for (const auto &primitive : primitives) {
            entries.push_back(BuildCollisionDebugEntry(primitive));
        }
    }

    for (const auto &gangway : gangways) {
        if (gangway == nullptr ||
            !gangway->ConnectsRoom(room) ||
            !collectedGangways.insert(gangway.get()).second) {
            continue;
        }

        AppendStaticCollisionEntries(
            gangway->GetStaticColliders(),
            "Gangway",
            Util::Color(93, 173, 226, 220),
            entries
        );
    }
}

CollisionDebugCameraState CaptureCollisionDebugCameraState(Camera *camera) {
    if (camera == nullptr) {
        return {};
    }

    return {
        camera->GetCooridinate(),
        camera->GetScale(),
        camera->GetRotation()
    };
}

} // namespace

CollisionDebugSnapshot BuildCollisionDebugSnapshot(
    const std::shared_ptr<BaseRoom> &currentRoom,
    const RoomTransitionSystem::DoorPassageContext &doorPassage,
    const std::vector<std::shared_ptr<Gangway>> &gangways,
    const std::vector<std::shared_ptr<Player>> &players,
    const std::vector<std::shared_ptr<Mob>> &mobs,
    const std::vector<std::shared_ptr<Bullet>> &bullets,
    Camera *attachCamera
) {
    CollisionDebugSnapshot snapshot;
    snapshot.cameraState = CaptureCollisionDebugCameraState(attachCamera);

    std::unordered_set<const void *> seenOwners;
    std::unordered_set<const Gangway *> collectedGangways;

    AppendRoomVisuals(currentRoom, gangways, snapshot.visualEntries, seenOwners);

    if (doorPassage.state == RoomTransitionSystem::DoorPassageState::Traversing &&
        doorPassage.targetRoom != nullptr &&
        doorPassage.targetRoom != currentRoom) {
        AppendRoomVisuals(doorPassage.targetRoom, gangways, snapshot.visualEntries, seenOwners);
    }

    for (const auto &player : players) {
        AppendDebugVisualEntry(player, snapshot.visualEntries, seenOwners);
    }

    for (const auto &mob : mobs) {
        AppendDebugVisualEntry(mob, snapshot.visualEntries, seenOwners);
    }

    for (const auto &bullet : bullets) {
        AppendDebugVisualEntry(bullet, snapshot.visualEntries, seenOwners);
    }

    AppendRoomColliders(currentRoom, gangways, snapshot.entries, collectedGangways);

    if (doorPassage.state == RoomTransitionSystem::DoorPassageState::Traversing &&
        doorPassage.targetRoom != nullptr &&
        doorPassage.targetRoom != currentRoom) {
        AppendRoomColliders(doorPassage.targetRoom, gangways, snapshot.entries, collectedGangways);
    }

    for (const auto &player : players) {
        AppendDynamicCollisionEntries(player, snapshot.entries);
    }

    for (const auto &mob : mobs) {
        AppendDynamicCollisionEntries(mob, snapshot.entries);
    }

    for (const auto &bullet : bullets) {
        AppendDynamicCollisionEntries(bullet, snapshot.entries);
    }

    return snapshot;
}
