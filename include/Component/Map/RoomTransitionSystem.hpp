#ifndef ROOM_TRANSITION_SYSTEM_HPP
#define ROOM_TRANSITION_SYSTEM_HPP

#include <cmath>
#include <memory>
#include <vector>

#include <glm/geometric.hpp>
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
    static DoorSide GetOppositeSide(DoorSide side) {
        switch (side) {
        case DoorSide::Top:
            return DoorSide::Bottom;
        case DoorSide::Right:
            return DoorSide::Left;
        case DoorSide::Bottom:
            return DoorSide::Top;
        case DoorSide::Left:
            return DoorSide::Right;
        }

        return DoorSide::Bottom;
    }

    static glm::vec2 GetInwardNormal(DoorSide side) {
        switch (side) {
        case DoorSide::Top:
            return {0.0F, -1.0F};
        case DoorSide::Right:
            return {-1.0F, 0.0F};
        case DoorSide::Bottom:
            return {0.0F, 1.0F};
        case DoorSide::Left:
            return {1.0F, 0.0F};
        }

        return {0.0F, 0.0F};
    }

    bool AreFacingSocketsAligned(
        const BaseRoom::PassageSocket &sourceSocket,
        const BaseRoom::PassageSocket &targetSocket
    ) const {
        if (targetSocket.side != GetOppositeSide(sourceSocket.side)) {
            return false;
        }

        switch (sourceSocket.side) {
        case DoorSide::Top:
            return std::abs(sourceSocket.worldCenter.x - targetSocket.worldCenter.x) <=
                       kPassageAlignmentTolerance &&
                   targetSocket.worldCenter.y > sourceSocket.worldCenter.y;

        case DoorSide::Right:
            return std::abs(sourceSocket.worldCenter.y - targetSocket.worldCenter.y) <=
                       kPassageAlignmentTolerance &&
                   targetSocket.worldCenter.x > sourceSocket.worldCenter.x;

        case DoorSide::Bottom:
            return std::abs(sourceSocket.worldCenter.x - targetSocket.worldCenter.x) <=
                       kPassageAlignmentTolerance &&
                   sourceSocket.worldCenter.y > targetSocket.worldCenter.y;

        case DoorSide::Left:
            return std::abs(sourceSocket.worldCenter.y - targetSocket.worldCenter.y) <=
                       kPassageAlignmentTolerance &&
                   sourceSocket.worldCenter.x > targetSocket.worldCenter.x;
        }

        return false;
    }

    bool HasRoomPassageBetween(
        const std::shared_ptr<BaseRoom> &sourceRoom,
        const std::shared_ptr<BaseRoom> &targetRoom,
        DoorSide &targetEntrySide
    ) const {
        if (sourceRoom == nullptr || targetRoom == nullptr) {
            return false;
        }

        const DoorSide candidateSides[] = {
            DoorSide::Top,
            DoorSide::Right,
            DoorSide::Bottom,
            DoorSide::Left
        };

        for (DoorSide sourceExitSide : candidateSides) {
            if (!sourceRoom->HasPassageOnSide(sourceExitSide)) {
                continue;
            }

            const DoorSide candidateTargetEntrySide = GetOppositeSide(sourceExitSide);
            if (!targetRoom->HasPassageOnSide(candidateTargetEntrySide)) {
                continue;
            }

            const BaseRoom::PassageSocket sourceSocket =
                sourceRoom->GetPassageSocket(sourceExitSide);
            const BaseRoom::PassageSocket targetSocket =
                targetRoom->GetPassageSocket(candidateTargetEntrySide);

            if (!this->AreFacingSocketsAligned(sourceSocket, targetSocket)) {
                continue;
            }

            targetEntrySide = candidateTargetEntrySide;
            return true;
        }

        return false;
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

        const BaseRoom::PassageSocket targetEntrySocket =
            this->m_DoorPassage.targetRoom->GetPassageSocket(this->m_DoorPassage.targetEntrySide);
        const float commitThreshold = std::max(
            0.0F,
            kDoorPassageCommitDepth - targetEntrySocket.wallThickness / 2.0F
        );
        const float entryDepth = glm::dot(
            playerPos - targetEntrySocket.worldCenter,
            GetInwardNormal(this->m_DoorPassage.targetEntrySide)
        );
        return entryDepth >= commitThreshold;
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
    static constexpr float kPassageAlignmentTolerance = 1.0F;

    std::shared_ptr<BaseRoom> m_CurrentRoom = nullptr;
    DoorPassageContext m_DoorPassage;
};

#endif
