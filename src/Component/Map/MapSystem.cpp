#include "Component/Map/MapSystem.hpp"

MapSystem::MapSystem() {}

MapSystem::MapSystem(std::vector<std::shared_ptr<MapPiece>> pieces) {
    this->m_Pieces = pieces;
}