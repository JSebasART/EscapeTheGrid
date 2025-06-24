#include <algorithm>
#include "player.hpp"

Player::Player(int cellSize_, int mapSize_, int startX, int startY): cellSize(cellSize_),mapSize(mapSize_), gridX(startX), gridY(startY)
{
    float half = cellSize * 0.5f;
    shape.setRadius(half);
    // pass a Vector2 instead of two floats:
    shape.setOrigin({ half, half });
    shape.setFillColor(sf::Color::Green);

    shape.setPosition({
        gridX * float(cellSize) + half,
        gridY * float(cellSize) + half
    });
}

void Player::handleEvent(const sf::Event& event)
{
    if (auto key = event.getIf<sf::Event::KeyPressed>()) {
        int dx = 0, dy = 0;
        switch (key->code) {
            case sf::Keyboard::Key::A:  dx = -1; break;
            case sf::Keyboard::Key::D: dx = +1; break;
            case sf::Keyboard::Key::W:    dy = -1; break;
            case sf::Keyboard::Key::S:  dy = +1; break;
            default:                  return;
        }

        gridX = std::clamp(gridX + dx, 0, mapSize - 1);
        gridY = std::clamp(gridY + dy, 0, mapSize - 1);

        shape.setPosition({
            gridX * cellSize + cellSize * 0.5f,
            gridY * cellSize + cellSize * 0.5f
        });
    }
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}
