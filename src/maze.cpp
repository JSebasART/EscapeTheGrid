#include "maze.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <raylib.h>    // Para TraceLog y LOG_WARNING, opcional

// Definición real del vector global
std::vector<std::vector<char>> maze;

void InitializeMaze() {
    maze.clear();
    std::ifstream file("resources/maze.txt");
    if (!file.is_open()) {
        TraceLog(LOG_WARNING, "No se pudo abrir maze.txt");
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Eliminar espacios y tabs
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty()) continue;

        // Quitar '{' inicial y '},' o '}' final
        if (line.front() == '{') line.erase(0,1);
        if (!line.empty() && (line.back() == ',' || line.back() == '}')) line.pop_back();
        if (line.empty()) continue;

        std::vector<char> row;
        std::stringstream ss(line);
        std::string token;

        // Separar por comas
        while (std::getline(ss, token, ',')) {
            if (token.empty()) continue;
            row.push_back(token[0]);
        }

        // Asegurar que todas las filas tengan el mismo largo si deseas
        maze.push_back(row);
    }

    if (maze.empty() || maze[0].empty()) {
        TraceLog(LOG_WARNING, "maze.txt vacío o mal formateado");
        exit(EXIT_FAILURE);
    }
}

void ResetMaze() {
    InitializeMaze();
}

std::pair<int, int> FindStartPosition() {
    for (int i = 0; i < (int)maze.size(); i++) {
        for (int j = 0; j < (int)maze[i].size(); j++) {
            if (maze[i][j] == 'S') return {i, j};
        }
    }
    return {0, 0};
}

std::pair<int, int> FindEndPosition() {
    for (int i = 0; i < (int)maze.size(); i++) {
        for (int j = 0; j < (int)maze[i].size(); j++) {
            if (maze[i][j] == 'G') return {i, j};
        }
    }
    return {0, 0};
}

bool IsValidPosition(int row, int col) {
    return row >= 0 && row < (int)maze.size()
        && col >= 0 && col < (int)maze[0].size();
}