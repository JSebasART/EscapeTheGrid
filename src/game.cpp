#include "game.hpp"
#include "maze.hpp"
#include "pathFinding.hpp"
#include "ui.hpp"

// Variables globales
GameScreen currentScreen = MENU;
GameScreen previousScreen = MENU;
Player player = {0, 0};
int selectedOption = 0;
int selectedWinOption = 0;
int stepCount = 0;
int itemsCollected = 0;
float energy = 0.0f;
bool canBreakWall = false;
int lastDirection = 0;
bool solvePath = false;
std::vector<std::pair<int, int>> solutionPath;

// Animación
Texture2D playerTexture;
int playerFrame = 0;
float animationTimer = 0.0f;
const float FRAME_TIME = 0.4f;

void InitializeGame() {
    InitializeMaze();
    auto start = FindStartPosition();
    player.row = start.first;
    player.col = start.second;
    solutionPath = FindPath(FindEndPosition());
    
    // Inicializar energía y coleccionables
    stepCount = 0;
    itemsCollected = 0;
    energy = 0.0f;
    canBreakWall = false;
    solvePath = false;

    playerTexture = LoadTexture("resources/spritesheet.png");
    if (playerTexture.id <= 0) {
        TraceLog(LOG_WARNING, "No se pudo cargar la textura del jugador!");
    }
}

void ResetGame() {
    ResetMaze();
    auto start = FindStartPosition();
    player.row = start.first;
    player.col = start.second;
    
    // Resetear solo estado del jugador, no del laberinto
    stepCount = 0;
    itemsCollected = 0;
    energy = 0.0f;
    canBreakWall = false;
    solutionPath = FindPath(FindEndPosition());
    solvePath = false;
}

void UpdateGame() {
    switch (currentScreen) {
        case MENU: UpdateMenu(); break;
        case GAME: UpdateGameScreen(); break;
        case PAUSE: UpdatePauseScreen(); break;
        case CONTROLS: UpdateControlsScreen(); break;
        case VICTORY: UpdateVictoryScreen(); break;
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    switch (currentScreen) {
        case MENU: DrawMenu(); break;
        case GAME: DrawGameScreen(); break;
        case PAUSE: DrawPauseScreen(); break;
        case CONTROLS: DrawControlsScreen(); break;
        case VICTORY: DrawVictoryScreen(); break;
    }
    
    EndDrawing();
}