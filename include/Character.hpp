#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>

#include "Util/GameObject.hpp"

class Character : public Util::GameObject {
public:
    explicit Character(const std::string& ImagePath);

    Character(const Character&) = delete;

    Character(Character&&) = delete;

    Character& operator=(const Character&) = delete;

    Character& operator=(Character&&) = delete;

    [[nodiscard]] const std::string& GetImagePath() const { return m_ImagePath; }

    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }

    [[nodiscard]] bool GetVisibility() const { return m_Visible; }

    void SetImage(const std::string& ImagePath);

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }

    void MoveByOffset(const glm::vec2& Offset) {
        const glm::vec2& previous_pos = GetPosition();
        SetPosition({previous_pos.x + Offset.x * m_Speed, previous_pos.y + Offset.y * m_Speed});
    }

    // TODO: Implement the collision detection
    [[nodiscard]] bool IfCollides(const std::shared_ptr<Character>& other) const {
        glm::vec2 t_pos = this->GetPosition();
        glm::vec2 t_size = this->GetScaledSize();

        glm::vec2 o_pos = other->GetPosition();
        glm::vec2 o_size = other->GetScaledSize();

        if (t_pos.x + t_size.x < o_pos.x || 
            t_pos.y + t_size.y < o_pos.y ||
            t_pos.x > o_pos.x + o_size.x || 
            t_pos.y > o_pos.y + o_size.y
        ) {
            return false;
        }

        return true;
    }

    // TODO: Add and implement more methods and properties as needed to finish Giraffe Adventure.

private:
    void ResetPosition() { m_Transform.translation = {0, 0}; }

    std::string m_ImagePath;

    int m_Speed = 5;
};


#endif //CHARACTER_HPP
