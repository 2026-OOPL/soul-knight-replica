#include "Component/Map/Room23x17.hpp"

#include <utility>

#include "Common/Constants.hpp"

namespace {

const glm::vec2 kRoom23x17Size = {
    23.0F * MAP_PIXEL_PER_BLOCK,
    17.0F * MAP_PIXEL_PER_BLOCK
};

} // namespace

Room23x17::Room23x17(RoomAssemblyConfig config)
    : BaseRoom(
          RESOURCE_DIR "/Map/Room/Room_23x17.png",
          kRoom23x17Size,
          std::move(config)
      ) {
}
