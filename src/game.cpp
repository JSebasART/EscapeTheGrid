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
Texture2D controlsTexture;

// Temáticas
Theme currentTheme = THEME_CASTLE;
ThemeAssets currentThemeAssets;

// Animación
Texture2D playerTexture;
int playerFrame = 0;
float animationTimer = 0.0f;
const float FRAME_TIME = 0.4f;

void LoadTheme(Theme theme) {
    // Descargar texturas anteriores si existen
    if (currentThemeAssets.wall.id != 0) UnloadTexture(currentThemeAssets.wall);
    if (currentThemeAssets.path.id != 0) UnloadTexture(currentThemeAssets.path);
    if (currentThemeAssets.start.id != 0) UnloadTexture(currentThemeAssets.start);
    if (currentThemeAssets.end.id != 0) UnloadTexture(currentThemeAssets.end);
    if (currentThemeAssets.item.id != 0) UnloadTexture(currentThemeAssets.item);

    std::string themePath;
    switch (theme) {
        case THEME_CASTLE: themePath = "resources/themes/castle/"; break;
        case THEME_FOREST: themePath = "resources/themes/forest/"; break;
        case THEME_DUNGEON: themePath = "resources/themes/dungeon/"; break;
        case THEME_ICE: themePath = "resources/themes/ice/"; break;
    }

    // Cargar nuevas texturas
    currentThemeAssets.wall = LoadTexture((themePath + "wall.png").c_str());
    currentThemeAssets.path = LoadTexture((themePath + "path.png").c_str());
    currentThemeAssets.start = LoadTexture((themePath + "start.png").c_str());
    currentThemeAssets.end = LoadTexture((themePath + "end.png").c_str());
    currentThemeAssets.item = LoadTexture((themePath + "item.png").c_str());

    // Verificar carga
    if (currentThemeAssets.wall.id == 0) TraceLog(LOG_WARNING, "No se pudo cargar textura de pared");
    if (currentThemeAssets.path.id == 0) TraceLog(LOG_WARNING, "No se pudo cargar textura de camino");
    if (currentThemeAssets.start.id == 0) TraceLog(LOG_WARNING, "No se pudo cargar textura de inicio");
    if (currentThemeAssets.end.id == 0) TraceLog(LOG_WARNING, "No se pudo cargar textura de fin");
    if (currentThemeAssets.item.id == 0) TraceLog(LOG_WARNING, "No se pudo cargar textura de ítem");

    currentTheme = theme;
}

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

    // Cargar tema inicial
    LoadTheme(THEME_CASTLE);

    playerTexture = LoadTexture("resources/player.png");
    if (playerTexture.id <= 0) {
        TraceLog(LOG_WARNING, "No se pudo cargar la textura del jugador!");
    }

    // Cargar textura de controles
    controlsTexture = LoadTexture("resources/controls.png");
    if (controlsTexture.id == 0) {
        TraceLog(LOG_WARNING, "No se pudo cargar la textura de controles!");
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

void CleanupGame() {
    // Descargar texturas de tema
    UnloadTexture(currentThemeAssets.wall);
    UnloadTexture(currentThemeAssets.path);
    UnloadTexture(currentThemeAssets.start);
    UnloadTexture(currentThemeAssets.end);
    UnloadTexture(currentThemeAssets.item);
    
    // Descargar otras texturas
    UnloadTexture(playerTexture);

    // Descargar textura de controles
    if (controlsTexture.id != 0) {
        UnloadTexture(controlsTexture);
    }
}