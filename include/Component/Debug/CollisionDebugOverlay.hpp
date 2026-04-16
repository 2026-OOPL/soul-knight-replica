#ifndef COMPONENT_DEBUG_COLLISION_DEBUG_OVERLAY_HPP
#define COMPONENT_DEBUG_COLLISION_DEBUG_OVERLAY_HPP

#include <memory>
#include <vector>

#include "Component/Debug/CollisionDebugSnapshot.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"

class SolidColorRectDrawable;

class CollisionDebugOverlay : public Util::GameObject {
public:
    CollisionDebugOverlay();

    void SetEnabled(bool enabled);
    void Sync(const CollisionDebugSnapshot &snapshot);

private:
    struct SpriteVisual {
        std::shared_ptr<Util::GameObject> imageNode;
    };

    struct EntryVisual {
        std::shared_ptr<SolidColorRectDrawable> fillDrawable;
        std::shared_ptr<SolidColorRectDrawable> borderDrawable;
        std::shared_ptr<Util::Text> labelDrawable;
        std::shared_ptr<Util::GameObject> fillNode;
        std::shared_ptr<Util::GameObject> topBorderNode;
        std::shared_ptr<Util::GameObject> bottomBorderNode;
        std::shared_ptr<Util::GameObject> leftBorderNode;
        std::shared_ptr<Util::GameObject> rightBorderNode;
        std::shared_ptr<Util::GameObject> labelNode;
    };

    void EnsureSpriteVisualCount(std::size_t count);
    void HideSpriteVisual(SpriteVisual &visual);
    void HideUnusedSpriteVisuals(std::size_t usedCount);
    void ApplySpriteVisual(
        SpriteVisual &visual,
        const CollisionDebugVisualEntry &entry
    );
    void EnsureVisualCount(std::size_t count);
    void HideVisual(EntryVisual &visual);
    void HideUnusedVisuals(std::size_t usedCount);
    void ApplyEntryVisual(
        EntryVisual &visual,
        const CollisionDebugEntry &entry,
        const CollisionDebugCameraState &cameraState
    );
    static glm::vec2 WorldToScreen(
        const glm::vec2 &worldPoint,
        const CollisionDebugCameraState &cameraState
    );

    std::vector<SpriteVisual> m_SpriteVisuals;
    std::vector<EntryVisual> m_EntryVisuals;
    bool m_Enabled = false;
};

#endif
