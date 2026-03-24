#ifndef FIGHT_CHAMBER_GEN
#define FIGHT_CHAMBER_GEN

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "MapBlueprint.hpp"

class FightChamberGen {
public:
    FightChamberGen(
        glm::vec2 start,
        glm::vec2 size,
        glm::vec2 position,
        std::shared_ptr<MapBlueprint> blueprint,
        std::shared_ptr<RandomChoose> random,
        int roomCount
    );

    ~FightChamberGen() = default;

    void Generate();

    void CreateRoom(glm::vec2 position);
    bool isCooridinateInBound(glm::vec2 position);
    glm::vec2 GetRandomNeighbor(glm::vec2 position);

protected:
    glm::vec2 m_Start;
    glm::vec2 m_MapGridSize;
    glm::vec2 m_MapGridPosition;

    int m_MapRoomCount;

    std::shared_ptr<MapBlueprint> m_Blueprint;
    std::shared_ptr<RandomChoose> m_RandomChoose;
};

#endif