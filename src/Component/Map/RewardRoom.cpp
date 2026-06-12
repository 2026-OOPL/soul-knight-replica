#include <array>
#include <memory>
#include <random>
#include "Component/Map/RewardRoom.hpp"

#include "Component/Map/MapSystem.hpp"
#include "Component/Prop/Chest.hpp"
#include "Component/Prop/ChestReward.hpp"
#include "Component/Weapon.hpp"
#include "Util/Image.hpp"

namespace {

constexpr char kRewardChestClosedSprite[] = RESOURCE_DIR "/Map/Box/TreasureChestClose.png";
constexpr char kRewardChestOpenSprite[] = RESOURCE_DIR "/Map/Box/TreasureChestOpen.png";
constexpr glm::vec2 kRewardChestRenderSize = {40.0F, 40.0F};
constexpr glm::vec2 kRewardChestBlockingSize = {28.0F, 20.0F};
constexpr glm::vec2 kRewardChestBlockingOffset = {0.0F, -6.0F};
constexpr float kRewardChestAutoOpenRange = 42.0F;

Chest::Visuals BuildRewardChestVisuals() {
    Chest::Visuals visuals;
    visuals.closedIdle = std::make_shared<Util::Image>(kRewardChestClosedSprite, false);
    visuals.openIdle = std::make_shared<Util::Image>(kRewardChestOpenSprite, false);
    return visuals;
}

std::shared_ptr<Weapon> CreateRewardWeapon() {
    static std::random_device randomDevice;
    static std::mt19937 randomEngine(randomDevice());
    static constexpr std::array<WeaponId, 4> kRewardWeaponPool = {
        WeaponId::BadPistol,
        WeaponId::Plunger,
        WeaponId::AK47,
        WeaponId::SniperRifle
    };
    static std::uniform_int_distribution<std::size_t> distribution(
        0,
        kRewardWeaponPool.size() - 1
    );
    static WeaponId lastRewardWeapon = WeaponId::Empty;

    WeaponId rewardWeapon = WeaponId::Empty;
    do {
        rewardWeapon = kRewardWeaponPool[distribution(randomEngine)];
    } while (
        rewardWeapon == lastRewardWeapon &&
        kRewardWeaponPool.size() > 1
    );

    lastRewardWeapon = rewardWeapon;
    return CreateWeaponById(rewardWeapon);
}

} // namespace

RewardRoom::RewardRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    const std::shared_ptr<RoomInfo> &info,
    float wallThickness
)
    : BaseRoom(
          absolutePosition,
          info->GetRoomType(),
          RoomPurpose::REWARD,
          doorConfig,
          BaseRoom::BuildWallConfigFromDoorConfig(
              doorConfig,
              wallThickness,
              info->GetRoomType(),
              info->GetRoomPurpose()
          )
      ) {
}

void RewardRoom::Initialize(MapSystem *mapSystem) {
    Chest::Config config;
    config.owningRoom = std::static_pointer_cast<BaseRoom>(shared_from_this());
    config.blockingSize = kRewardChestBlockingSize;
    config.blockingOffset = kRewardChestBlockingOffset;
    config.renderSize = kRewardChestRenderSize;
    config.autoOpenRange = kRewardChestAutoOpenRange;
    config.zIndex = 3;
    config.visuals = BuildRewardChestVisuals();
    config.reward = std::make_shared<WeaponChestReward>(CreateRewardWeapon);

    mapSystem->AddProp(
        std::make_shared<Chest>(this->GetAbsoluteTranslation(), std::move(config))
    );
}
