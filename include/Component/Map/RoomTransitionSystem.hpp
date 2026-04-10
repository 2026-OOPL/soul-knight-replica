#ifndef ROOM_TRANSITION_SYSTEM_HPP
#define ROOM_TRANSITION_SYSTEM_HPP

#include <cmath>
#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Map/BaseRoom.hpp"

class RoomTransitionSystem {
public:
    enum class DoorPassageState {
        Idle,
        Traversing
    };

    struct DoorPassageContext {
        DoorPassageState state = DoorPassageState::Idle;
        std::shared_ptr<BaseRoom> sourceRoom = nullptr;
        std::shared_ptr<BaseRoom> targetRoom = nullptr;
        DoorSide targetEntrySide = DoorSide::Bottom;
    };

    RoomTransitionSystem() = default;

    std::shared_ptr<BaseRoom> GetCurrentRoom() const {
        return this->m_CurrentRoom;
    }

    const DoorPassageContext &GetDoorPassage() const {
        return this->m_DoorPassage;
    }

    bool HasActiveDoorPassage() const {
        return this->m_DoorPassage.state == DoorPassageState::Traversing;
    }

    void PrepareDoorPassage(
        const glm::vec2 &playerPos,
        const std::vector<std::shared_ptr<BaseRoom>> &rooms
    ) {
        if (this->m_DoorPassage.state == DoorPassageState::Traversing ||
            this->m_CurrentRoom == nullptr) {
            return;
        }

        const std::shared_ptr<BaseRoom> nextRoom =
            this->FindRoomByPlayerPosition(playerPos, rooms);
        if (nextRoom == nullptr || nextRoom == this->m_CurrentRoom) {
            return;
        }

        this->TryStartDoorPassage(nextRoom);
    }

    void UpdateCurrentRoom(
        const glm::vec2 &playerPos,
        const std::vector<std::shared_ptr<BaseRoom>> &rooms
    ) {
        const std::shared_ptr<BaseRoom> nextRoom =
            this->FindRoomByPlayerPosition(playerPos, rooms);

        if (this->m_DoorPassage.state == DoorPassageState::Traversing) {
            if (this->m_DoorPassage.sourceRoom != nullptr &&
                this->m_DoorPassage.sourceRoom->IsPlayerInside(playerPos)) {
                this->CancelDoorPassage();
                return;
            }

            if (this->HasCommittedDoorPassage(playerPos)) {
                this->CommitDoorPassage();
                return;
            }

            if (nextRoom == nullptr || nextRoom == this->m_DoorPassage.targetRoom) {
                return;
            }

            this->CancelDoorPassage();
        }

        if (nextRoom == this->m_CurrentRoom || nextRoom == nullptr) {
            return;
        }

        if (this->m_CurrentRoom != nullptr && this->TryStartDoorPassage(nextRoom)) {
            if (this->HasCommittedDoorPassage(playerPos)) {
                this->CommitDoorPassage();
            }
            return;
        }

        if (this->m_CurrentRoom != nullptr) {
            this->m_CurrentRoom->OnPlayerLeave();
        }

        this->m_CurrentRoom = nextRoom;

        if (this->m_CurrentRoom != nullptr) {
            this->m_CurrentRoom->OnPlayerEnter();
        }
    }

    std::shared_ptr<BaseRoom> FindRoomByPlayerPosition(
        const glm::vec2 &playerPos,
        const std::vector<std::shared_ptr<BaseRoom>> &rooms
    ) const {
        for (const auto &room : rooms) {
            if (room != nullptr && room->IsPlayerInside(playerPos)) {
                return room;
            }
        }

        return nullptr;
    }

private:
    bool HasRoomPassageBetween(
        const std::shared_ptr<BaseRoom> &sourceRoom,
        const std::shared_ptr<BaseRoom> &targetRoom,
        DoorSide &targetEntrySide
    ) const {
        if (sourceRoom == nullptr || targetRoom == nullptr) {
            return false;
        }

        const glm::vec2 delta =
            targetRoom->GetAbsoluteTranslation() - sourceRoom->GetAbsoluteTranslation();
        DoorSide sourceExitSide = DoorSide::Bottom;

        if (std::abs(delta.x) > std::abs(delta.y)) {
            if (std::abs(delta.y) > kRoomAlignmentTolerance ||
                std::abs(delta.x) <= kRoomAlignmentTolerance) {
                return false;
            }

            if (delta.x > 0.0F) {
                sourceExitSide = DoorSide::Right;
                targetEntrySide = DoorSide::Left;
            } else {
                sourceExitSide = DoorSide::Left;
                targetEntrySide = DoorSide::Right;
            }
        } else {
            if (std::abs(delta.x) > kRoomAlignmentTolerance ||
                std::abs(delta.y) <= kRoomAlignmentTolerance) {
                return false;
            }

            if (delta.y > 0.0F) {
                sourceExitSide = DoorSide::Top;
                targetEntrySide = DoorSide::Bottom;
            } else {
                sourceExitSide = DoorSide::Bottom;
                targetEntrySide = DoorSide::Top;
            }
        }

        return sourceRoom->HasPassageOnSide(sourceExitSide) &&
               targetRoom->HasPassageOnSide(targetEntrySide);
    }

    bool TryStartDoorPassage(const std::shared_ptr<BaseRoom> &targetRoom) {
        if (this->m_CurrentRoom == nullptr ||
            targetRoom == nullptr ||
            targetRoom == this->m_CurrentRoom) {
            return false;
        }

        DoorSide targetEntrySide = DoorSide::Bottom;
        if (!this->HasRoomPassageBetween(this->m_CurrentRoom, targetRoom, targetEntrySide)) {
            return false;
        }

        this->m_DoorPassage.state = DoorPassageState::Traversing;
        this->m_DoorPassage.sourceRoom = this->m_CurrentRoom;
        this->m_DoorPassage.targetRoom = targetRoom;
        this->m_DoorPassage.targetEntrySide = targetEntrySide;
        return true;
    }

    bool HasCommittedDoorPassage(const glm::vec2 &playerPos) const {
        if (this->m_DoorPassage.state != DoorPassageState::Traversing ||
            this->m_DoorPassage.targetRoom == nullptr ||
            !this->m_DoorPassage.targetRoom->IsPlayerInside(playerPos)) {
            return false;
        }

        const glm::vec2 roomCenter = this->m_DoorPassage.targetRoom->GetAbsoluteTranslation();
        const glm::vec2 roomHalfSize = this->m_DoorPassage.targetRoom->GetRoomSize() / 2.0F;

        switch (this->m_DoorPassage.targetEntrySide) {
        case DoorSide::Top:
            return playerPos.y <= roomCenter.y + roomHalfSize.y - kDoorPassageCommitDepth;

        case DoorSide::Right:
            return playerPos.x <= roomCenter.x + roomHalfSize.x - kDoorPassageCommitDepth;

        case DoorSide::Bottom:
            return playerPos.y >= roomCenter.y - roomHalfSize.y + kDoorPassageCommitDepth;

        case DoorSide::Left:
            return playerPos.x >= roomCenter.x - roomHalfSize.x + kDoorPassageCommitDepth;
        }

        return false;
    }

    void CommitDoorPassage() {
        if (this->m_DoorPassage.state != DoorPassageState::Traversing ||
            this->m_DoorPassage.targetRoom == nullptr) {
            return;
        }

        if (this->m_CurrentRoom != nullptr) {
            this->m_CurrentRoom->OnPlayerLeave();
        }

        this->m_CurrentRoom = this->m_DoorPassage.targetRoom;
        this->m_CurrentRoom->OnPlayerEnter();
        this->CancelDoorPassage();
    }

    void CancelDoorPassage() {
        this->m_DoorPassage = DoorPassageContext{};
    }

    static constexpr float kDoorPassageCommitDepth = 32.0F;
    static constexpr float kRoomAlignmentTolerance = 1.0F;

    std::shared_ptr<BaseRoom> m_CurrentRoom = nullptr;
    DoorPassageContext m_DoorPassage;
};

#endif
