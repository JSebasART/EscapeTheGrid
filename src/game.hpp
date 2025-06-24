#pragma once
#include "raylib.h"
#include <vector>
#include <utility>
#include <string>

// Constantes globales
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 860;
const int CELL_SIZE = 80;
const int UI_HEIGHT = 60;

// Colores
const Color PLAYER_COLOR = BLUE;
const Color WALL_COLOR = DARKGRAY;
const Color PATH_COLOR = LIGHTGRAY;
const Color START_COLOR = GREEN;
const Color END_COLOR = RED;
const Color ITEM_COLOR = ORANGE;
const Color BUTTON_COLOR = LIGHTGRAY;
const Color BUTTON_SELECTED = SKYBLUE;
const Color VICTORY_COLOR = Color{46, 204, 113, 255};
const Color SOLUTION_COLOR = Color{255, 107, 107, 255};
const Color ENERGY_FILL_COLOR = Color{0, 228, 48, 255};
const Color ENERGY_READY_COLOR = Color{255, 0, 0, 255};

// Enumeración de temáticas
enum Theme {
    THEME_CASTLE,
    THEME_FOREST,
    THEME_DUNGEON,
    THEME_ICE
};

// Estructura para almacenar texturas de una temática
struct ThemeAssets {
    Texture2D wall;
    Texture2D path;
    Texture2D start;
    Texture2D end;
    Texture2D item;
};

// Estados del juego
enum GameScreen {
    MENU,
    GAME,
    PAUSE,
    CONTROLS,
    VICTORY
};

// Jugador
struct Player {
    int row;
    int col;
    int colDirection = 1;
};

// Variables globales externas
extern GameScreen currentScreen;
extern GameScreen previousScreen;
extern Player player;
extern int selectedOption;
extern int selectedWinOption;
extern int stepCount;
extern int itemsCollected;
extern float energy;
extern bool canBreakWall;
extern int lastDirection;
extern bool solvePath;
extern std::vector<std::pair<int, int>> solutionPath;
extern Theme currentTheme;
extern ThemeAssets currentThemeAssets;
extern Texture2D controlsTexture;

// Variables para animación
extern Texture2D playerTexture;
extern int playerFrame;
extern float animationTimer;
extern const float FRAME_TIME;

// Funciones principales
void InitializeGame();
void UpdateGame();
void DrawGame();
void ResetGame();