#ifndef CASTING_TEXT_HPP
#define CASTING_TEXT_HPP

#include "Common/MapObject.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include <glm/ext/vector_float2.hpp>
#include <memory>

class CastingText : public MapObject, public Util::GameObject {
public:
    CastingText(std::string content);
    CastingText(std::string content, glm::vec2 position);
    CastingText(std::string content, glm::vec2 position, double size);

    Util::Transform GetObjectTransform() const override;
private:
    std::shared_ptr<Util::Text> m_Text;

};

#endif