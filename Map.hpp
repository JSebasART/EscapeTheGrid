#pragma once

#include <SFML/Graphics.hpp>

class Map {
public:
    Map(int cellSize_, int size_): cellSize(cellSize_), size(size_) {
        grid.setPrimitiveType(sf::PrimitiveType::Lines);
        for (int x = 0; x <= size; ++x) {
            float vx = x * cellSize;
            grid.append({{vx,   0},                   sf::Color(150,150,150)});
            grid.append({{vx, float(size * cellSize)}, sf::Color(150,150,150)});
        }
        for (int y = 0; y <= size; ++y) {
            float hy = y * cellSize;
            grid.append({{   0, hy}, sf::Color(150,150,150)});
            grid.append({{float(size * cellSize), hy}, sf::Color(150,150,150)});
        }
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(grid);
    }
    
private:
    int cellSize;
    int size;
    sf::VertexArray grid;
};
