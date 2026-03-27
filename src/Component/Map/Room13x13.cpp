#include "Component/Map/Room13x13.hpp"

#include <utility>

#include "Common/Constants.hpp"

namespace {

const glm::vec2 kRoom13x13Size = {
    13.0F * MAP_PIXEL_PER_BLOCK,
    13.0F * MAP_PIXEL_PER_BLOCK
};

} // namespace

Room13x13::Room13x13(RoomAssemblyConfig config)
    : BaseRoom(
          RESOURCE_DIR "/Map/Room/Room_13x13.png",
          kRoom13x13Size,
          std::move(config)
      ) {
}
