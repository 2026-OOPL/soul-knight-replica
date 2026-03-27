#include "Component/Map/Room17x17.hpp"

#include <utility>

#include "Common/Constants.hpp"

namespace {

const glm::vec2 kRoom17x17Size = {
    17.0F * MAP_PIXEL_PER_BLOCK,
    17.0F * MAP_PIXEL_PER_BLOCK
};

} // namespace

Room17x17::Room17x17(RoomAssemblyConfig config)
    : BaseRoom(
          RESOURCE_DIR "/Map/Room/Room_17x17.png",
          kRoom17x17Size,
          std::move(config)
      ) {
}
