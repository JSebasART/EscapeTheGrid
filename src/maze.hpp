#pragma once
#include <vector>
#include <utility>

// Laberinto
extern std::vector<std::vector<char>> maze;
extern std::vector<std::vector<char>> originalMaze;

// Funciones de laberinto
void InitializeMaze();
void ResetMaze();
std::pair<int, int> FindStartPosition();
std::pair<int, int> FindEndPosition();
bool IsValidPosition(int row, int col);