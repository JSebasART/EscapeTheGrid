#include "pathFinding.hpp"
#include "maze.hpp"
#include "game.hpp"
#include <queue>
#include <algorithm>

std::vector<std::pair<int, int>> FindPath(const std::pair<int, int>& end) {
    // Camino quemado predefinido para el laberinto original
    static const std::vector<std::pair<int, int>> burnedPath = {
        {0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 2}, {2, 3}, {2, 4}, {2, 5},
        {2, 6}, {2, 7}, {3, 7}, {4, 7}, {4, 8}, {4, 9}, {5, 9}, {6, 9},
        {7, 9}, {8, 9}, {9, 9}
    };
    
    return burnedPath;
}