#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player(int cellSize, int mapSize, int startX = 0, int startY = 0);

    void handleEvent(const sf::Event& event);

    void draw(sf::RenderWindow& window);

private:
    sf::CircleShape shape;
    int cellSize, mapSize;
    int gridX, gridY;
};
