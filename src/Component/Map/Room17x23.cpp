#include "Component/Map/Room17x23.hpp"

#include <utility>

#include "Common/Constants.hpp"

namespace {

const glm::vec2 kRoom17x23Size = {
    17.0F * MAP_PIXEL_PER_BLOCK,
    23.0F * MAP_PIXEL_PER_BLOCK
};

} // namespace

Room17x23::Room17x23(RoomAssemblyConfig config)
    : BaseRoom(
          RESOURCE_DIR "/Map/Room/Room_17x23.png",
          kRoom17x23Size,
          std::move(config)
      ) {
}
