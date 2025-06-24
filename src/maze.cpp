#include "maze.hpp"

std::vector<std::vector<char>> maze;
typedef std::vector<std::vector<char>>::size_type size_type;
std::vector<std::vector<char>> originalMaze = {
    {'S', '.', '.', '#', '.', '.', '.', '.', '.', 'G'},
    {'#', '#', '.', '#', '.', '#', '#', '#', '.', '#'},
    {'.', '.', '.', '.', '.', '.', '.', '#', '.', '#'},
    {'.', '#', '#', '#', '#', '#', '.', '#', '.', '#'},
    {'.', '.', '.', '.', '.', '#', '.', '#', '.', '.'},
    {'.', '#', '.', '#', '.', '#', '.', '#', '#', '.'},
    {'.', '#', '.', '#', '.', '#', '.', '.', '.', '.'},
    {'.', '#', '.', '#', '.', '.', '.', '#', '#', '#'},
    {'.', '#', '.', '.', '.', '#', '.', '.', '.', '.'},
    {'.', '.', '.', '#', '.', '.', '.', '#', 'K', '#'}};

void InitializeMaze() {
    maze = originalMaze;
}

void ResetMaze() {
    maze = originalMaze;
}

std::pair<int, int> FindStartPosition() {
    for (size_type i = 0; i < maze.size(); i++) {
        for (size_type j = 0; j < maze[i].size(); j++) {
            if (maze[i][j] == 'S') {
                return {static_cast<int>(i), static_cast<int>(j)};
            }
        }
    }
    return {0, 0};
}

std::pair<int, int> FindEndPosition() {
    for (size_type i = 0; i < maze.size(); i++) {
        for (size_type j = 0; j < maze[i].size(); j++) {
            if (maze[i][j] == 'G') {
                return {static_cast<int>(i), static_cast<int>(j)};
            }
        }
    }
    return {0, 0};
}

bool IsValidPosition(int row, int col) {
    return (row >= 0) && (row < static_cast<int>(maze.size())) && 
           (col >= 0) && (col < static_cast<int>(maze[0].size()));
}