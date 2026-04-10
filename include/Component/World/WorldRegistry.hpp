#ifndef WORLD_REGISTRY_HPP
#define WORLD_REGISTRY_HPP

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Util/GameObject.hpp"

#include "Component/Bullet.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Player/Player.hpp"

class WorldRegistry {
public:
    WorldRegistry() = default;

    explicit WorldRegistry(Util::GameObject *root)
        : m_Root(root) {
    }

    void SetRoot(Util::GameObject *root) {
        this->m_Root = root;
    }

    void AddPlayer(const std::shared_ptr<Player> &player) {
        this->AddEntity(player, this->m_Players, "player");
    }

    void RemovePlayer(const std::shared_ptr<Player> &player) {
        this->RemoveEntity(player, this->m_Players, "player");
    }

    const std::vector<std::shared_ptr<Player>> &GetPlayers() const {
        return this->m_Players;
    }

    void AddRoom(const std::shared_ptr<BaseRoom> &room) {
        this->AddEntity(room, this->m_Rooms, "room");
    }

    void RemoveRoom(const std::shared_ptr<BaseRoom> &room) {
        this->RemoveEntity(room, this->m_Rooms, "room");
    }

    const std::vector<std::shared_ptr<BaseRoom>> &GetRooms() const {
        return this->m_Rooms;
    }

    void AddGangway(const std::shared_ptr<Gangway> &gangway) {
        this->AddEntity(gangway, this->m_Gangways, "gangway");
    }

    void RemoveGangway(const std::shared_ptr<Gangway> &gangway) {
        this->RemoveEntity(gangway, this->m_Gangways, "gangway");
    }

    const std::vector<std::shared_ptr<Gangway>> &GetGangways() const {
        return this->m_Gangways;
    }

    void AddMob(const std::shared_ptr<Character> &mob) {
        this->AddEntity(mob, this->m_Mobs, "mob");
    }

    void RemoveMob(const std::shared_ptr<Character> &mob) {
        this->RemoveEntity(mob, this->m_Mobs, "mob");
    }

    const std::vector<std::shared_ptr<Character>> &GetMobs() const {
        return this->m_Mobs;
    }

    void AddBullet(const std::shared_ptr<Bullet> &bullet) {
        this->AddEntity(bullet, this->m_Bullets, "bullet");
    }

    void RemoveBullet(const std::shared_ptr<Bullet> &bullet) {
        this->RemoveEntity(bullet, this->m_Bullets, "bullet");
    }

    const std::vector<std::shared_ptr<Bullet>> &GetBullets() const {
        return this->m_Bullets;
    }

private:
    template <typename T>
    void AddEntity(
        const std::shared_ptr<T> &entity,
        std::vector<std::shared_ptr<T>> &container,
        const char *entityName
    ) {
        if (entity == nullptr) {
            throw std::runtime_error("You are trying to add a null " +
                                     std::string(entityName));
        }

        if (std::find(container.begin(), container.end(), entity) != container.end()) {
            return;
        }

        if (this->m_Root != nullptr) {
            this->m_Root->AddChild(entity);
        }

        container.push_back(entity);
    }

    template <typename T>
    void RemoveEntity(
        const std::shared_ptr<T> &entity,
        std::vector<std::shared_ptr<T>> &container,
        const char *entityName
    ) {
        if (entity == nullptr) {
            throw std::runtime_error("You are trying to erase a null " +
                                     std::string(entityName));
        }

        const auto found = std::find(container.begin(), container.end(), entity);
        if (found == container.end()) {
            return;
        }

        if (this->m_Root != nullptr) {
            this->m_Root->RemoveChild(entity);
        }

        container.erase(found);
    }

    Util::GameObject *m_Root = nullptr;
    std::vector<std::shared_ptr<Player>> m_Players;
    std::vector<std::shared_ptr<BaseRoom>> m_Rooms;
    std::vector<std::shared_ptr<Gangway>> m_Gangways;
    std::vector<std::shared_ptr<Character>> m_Mobs;
    std::vector<std::shared_ptr<Bullet>> m_Bullets;
};

#endif
