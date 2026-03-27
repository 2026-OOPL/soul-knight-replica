#include "Component/Map/Room15x15.hpp"

#include <utility>

#include "Common/Constants.hpp"

namespace {

const glm::vec2 kRoom15x15Size = {
    15.0F * MAP_PIXEL_PER_BLOCK,
    15.0F * MAP_PIXEL_PER_BLOCK
};

} // namespace

Room15x15::Room15x15(RoomAssemblyConfig config)
    : BaseRoom(
          RESOURCE_DIR "/Map/Room/Room_15x15.png",
          kRoom15x15Size,
          std::move(config)
      ) {
}
