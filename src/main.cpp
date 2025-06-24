#include "game.hpp"
#include "maze.hpp"
#include "ui.hpp"

int main() {
    // Inicialización
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "La Casa de los Trucos");
    SetExitKey(0);
    SetTargetFPS(60);
    
    InitializeGame();
    
    // Calcular offsets del laberinto
    mazeOffsetX = (SCREEN_WIDTH - (static_cast<int>(maze[0].size()) * CELL_SIZE)) / 2;
    mazeOffsetY = UI_HEIGHT + (SCREEN_HEIGHT - UI_HEIGHT - (static_cast<int>(maze.size()) * CELL_SIZE)) / 2;

    // Bucle principal
    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    CloseWindow();
    UnloadTexture(playerTexture);
    return 0;
}