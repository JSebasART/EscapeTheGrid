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
extern bool isAutoSolving;
extern int autoStepIndex;
extern float autoMoveTimer;
extern int stepsSinceLastChange;
extern const int DISAPPEAR_INTERVAL;
extern const int WALLS_TO_REMOVE;
extern bool wallsRemoved;
extern bool hasBrokenWall;
extern const int MAX_ENERGY_STEPS; 

extern Texture2D menuBackgroundTexture;
extern Texture2D victoryBgTexture;

extern Texture2D playerUp;
extern Texture2D playerDown;
extern Texture2D playerLeft;
extern Texture2D playerRight;

// Funciones principales
void InitializeGame();
void UpdateGame();
void DrawGame();
void ResetGame();
void RemoveRandomWalls(int count);
void BreakWallAt(int row, int col);