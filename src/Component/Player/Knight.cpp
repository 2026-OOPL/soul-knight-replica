#include "Component/Player/Knight.hpp"

#include <algorithm>
#include <utility>

#include "Component/Player/Player.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"


Knight::Knight() : Player(
    KnightPlayer::STAND_SPRITES,
    KnightPlayer::WALK_SPRITES,
    KnightPlayer::DIE_SPRITES
) {}