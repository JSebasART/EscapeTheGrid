#include "raylib.h"
#include <vector>

// Constantes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int CELL_SIZE = 80;
const Color PLAYER_COLOR = BLUE;
const Color WALL_COLOR = DARKGRAY;
const Color PATH_COLOR = LIGHTGRAY;
const Color START_COLOR = GREEN;
const Color END_COLOR = RED;
const Color ITEM_COLOR = GOLD;
const Color BUTTON_COLOR = LIGHTGRAY;
const Color BUTTON_SELECTED = SKYBLUE;

enum GameScreen
{
    MENU,
    GAME,
    PAUSE,
    CONTROLS
};

// Representación del laberinto
std::vector<std::vector<char>> maze = {
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

// Definir el tipo para los tamaños
typedef std::vector<std::vector<char>>::size_type size_type;

struct Player
{
    int row;
    int col;
};

int main()
{
    // Inicializar ventana
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "La Casa de los Trucos");
    SetTargetFPS(60);

    // Estado del juego
    GameScreen currentScreen = MENU;
    Player player = {0, 0};           // Posición inicial (S)
    int selectedOption = 0;           // Opción seleccionada en menús
    GameScreen previousScreen = MENU; // Pantalla anterior para volver de CONTROLS

    // Encontrar la posición inicial (S) usando size_type
    for (size_type i = 0; i < maze.size(); i++)
    {
        for (size_type j = 0; j < maze[i].size(); j++)
        {
            if (maze[i][j] == 'S')
            {
                player.row = static_cast<int>(i);
                player.col = static_cast<int>(j);
            }
        }
    }

    // Coordenadas para centrar el laberinto
    int mazeOffsetX = (SCREEN_WIDTH - (static_cast<int>(maze[0].size()) * CELL_SIZE)) / 2;
    int mazeOffsetY = (SCREEN_HEIGHT - (static_cast<int>(maze.size()) * CELL_SIZE)) / 2;

    // Bucle principal del juego
    while (!WindowShouldClose())
    {
        // Actualización del juego
        //---------------------------------------------------------------------
        switch (currentScreen)
        {
        case MENU:
            // Navegación con flechas o WASD
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                selectedOption = (selectedOption + 1) % 3;
            }
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                selectedOption = (selectedOption + 2) % 3; // (current -1 + 3) % 3
            }

            // Selección con ENTER
            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selectedOption)
                {
                case 0:
                    currentScreen = GAME;
                    break; // PLAY
                case 1:
                    previousScreen = MENU;
                    currentScreen = CONTROLS;
                    break; // CONTROLS
                case 2:
                    CloseWindow();
                    return 0; // EXIT
                }
            }
            break;

        case GAME:
            // Movimiento del jugador
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                if (player.row > 0 && maze[player.row - 1][player.col] != '#')
                {
                    player.row--;
                }
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                if (player.row < static_cast<int>(maze.size()) - 1 &&
                    maze[player.row + 1][player.col] != '#')
                {
                    player.row++;
                }
            }
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
            {
                if (player.col > 0 && maze[player.row][player.col - 1] != '#')
                {
                    player.col--;
                }
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
            {
                if (player.col < static_cast<int>(maze[0].size()) - 1 &&
                    maze[player.row][player.col + 1] != '#')
                {
                    player.col++;
                }
            }

            // Pausa con ESC
            if (IsKeyPressed(KEY_ESCAPE))
            {
                selectedOption = 0; // Resetear opción seleccionada
                currentScreen = PAUSE;
            }

            // Verificar si llegó a la meta
            if (maze[player.row][player.col] == 'G')
            {
                currentScreen = MENU;
                player = {0, 0};
            }
            break;

        case PAUSE:
            // Navegación con flechas o WASD
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                selectedOption = (selectedOption + 1) % 3;
            }
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                selectedOption = (selectedOption + 2) % 3;
            }

            // Selección con ENTER
            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selectedOption)
                {
                case 0:
                    currentScreen = GAME;
                    break; // CONTINUE
                case 1:
                    previousScreen = PAUSE;
                    currentScreen = CONTROLS;
                    break; // CONTROLS
                case 2:
                    currentScreen = MENU; // EXIT TO MENU
                    player = {0, 0};
                    break;
                }
            }

            // Volver al juego con ESC
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = GAME;
            }
            break;

        // MANEJAR EL CASO CONTROLS EN EL SWITCH DE ACTUALIZACIÓN
        case CONTROLS:
            // Solo hay una opción (VOLVER), seleccionada por defecto
            selectedOption = 0;

            // Selección con ENTER o volver con ESC
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = previousScreen;
            }
            break;
        }
        //---------------------------------------------------------------------

        // Dibujado
        //---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScreen)
        {
        case MENU:
            // Título
            DrawText("ESCAPE THE GRID", SCREEN_WIDTH / 2 - MeasureText("ESCAPE THE GRID", 50) / 2, 150, 50, DARKGRAY);

            // Botón PLAY
            DrawRectangle(SCREEN_WIDTH / 2 - 100, 300, 200, 60,
                          selectedOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("PLAY", SCREEN_WIDTH / 2 - MeasureText("PLAY", 30) / 2, 315, 30,
                     selectedOption == 0 ? WHITE : DARKGRAY);

            // Botón CONTROLS
            DrawRectangle(SCREEN_WIDTH / 2 - 100, 380, 200, 60,
                          selectedOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("CONTROLS", SCREEN_WIDTH / 2 - MeasureText("CONTROLS", 30) / 2, 395, 30,
                     selectedOption == 1 ? WHITE : DARKGRAY);

            // Botón EXIT
            DrawRectangle(SCREEN_WIDTH / 2 - 100, 460, 200, 60,
                          selectedOption == 2 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("EXIT", SCREEN_WIDTH / 2 - MeasureText("EXIT", 30) / 2, 475, 30,
                     selectedOption == 2 ? WHITE : DARKGRAY);

            // Instrucciones de navegación
            DrawText("Use ARROWS/WASD to navigate, ENTER to select",
                     SCREEN_WIDTH / 2 - MeasureText("Use ARROWS/WASD to navigate, ENTER to select", 20) / 2,
                     550, 20, DARKGRAY);
            break;

        case GAME:
            // Dibujar laberinto
            for (size_type i = 0; i < maze.size(); i++)
            {
                for (size_type j = 0; j < maze[i].size(); j++)
                {
                    Color cellColor;

                    switch (maze[i][j])
                    {
                    case '#':
                        cellColor = WALL_COLOR;
                        break;
                    case 'S':
                        cellColor = START_COLOR;
                        break;
                    case 'G':
                        cellColor = END_COLOR;
                        break;
                    case 'K':
                        cellColor = ITEM_COLOR;
                        break;
                    default:
                        cellColor = PATH_COLOR;
                    }

                    DrawRectangle(
                        mazeOffsetX + static_cast<int>(j) * CELL_SIZE,
                        mazeOffsetY + static_cast<int>(i) * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE,
                        cellColor);

                    // Borde de la celda
                    DrawRectangleLines(
                        mazeOffsetX + static_cast<int>(j) * CELL_SIZE,
                        mazeOffsetY + static_cast<int>(i) * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE,
                        BLACK);
                }
            }

            // Dibujar jugador
            DrawCircle(
                mazeOffsetX + player.col * CELL_SIZE + CELL_SIZE / 2,
                mazeOffsetY + player.row * CELL_SIZE + CELL_SIZE / 2,
                CELL_SIZE / 3,
                PLAYER_COLOR);
            break;

        case PAUSE:
            // Fondo semitransparente
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));

            // Panel de pausa
            DrawRectangle(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 150, 300, 300, LIGHTGRAY);
            DrawText("PAUSE", SCREEN_WIDTH / 2 - MeasureText("PAUSE", 40) / 2, SCREEN_HEIGHT / 2 - 130, 40, DARKGRAY);

            // Botón CONTINUE
            DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 60, 200, 50,
                          selectedOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("CONTINUE", SCREEN_WIDTH / 2 - MeasureText("CONTINUE", 20) / 2,
                     SCREEN_HEIGHT / 2 - 45, 20, selectedOption == 0 ? WHITE : DARKGRAY);

            // Botón CONTROLS
            DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 50,
                          selectedOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("CONTROLS", SCREEN_WIDTH / 2 - MeasureText("CONTROLS", 20) / 2,
                     SCREEN_HEIGHT / 2 + 15, 20, selectedOption == 1 ? WHITE : DARKGRAY);

            // Botón EXIT TO MENU
            DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 200, 50,
                          selectedOption == 2 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("MAIN MENU", SCREEN_WIDTH / 2 - MeasureText("MAIN MENU", 20) / 2,
                     SCREEN_HEIGHT / 2 + 75, 20, selectedOption == 2 ? WHITE : DARKGRAY);

            // Instrucciones de navegación
            DrawText("Use ARROWS/WASD to navigate, ENTER to select, ESC to resume",
                     SCREEN_WIDTH / 2 - MeasureText("Use ARROWS/WASD to navigate, ENTER to select, ESC to resume", 20) / 2,
                     SCREEN_HEIGHT / 2 + 150, 20, DARKGRAY);
            break;

        case CONTROLS:
            // Fondo
            ClearBackground(RAYWHITE);

            // Título
            DrawText("CONTROLS", SCREEN_WIDTH / 2 - MeasureText("CONTROLS", 50) / 2, 100, 50, DARKGRAY);

            // Instrucciones
            DrawText("PLAYER MOVEMENT:", 100, 200, 30, DARKGRAY);
            DrawText("- Arrow keys: Up, Down, Left, Right", 120, 250, 25, DARKGRAY);
            DrawText("- WASD: Alternative movement keys", 120, 290, 25, DARKGRAY);

            DrawText("MENU NAVIGATION:", 100, 350, 30, DARKGRAY);
            DrawText("- Arrow keys/WASD: Move between options", 120, 400, 25, DARKGRAY);
            DrawText("- ENTER: Select option", 120, 440, 25, DARKGRAY);

            DrawText("GAME CONTROLS:", 100, 500, 30, DARKGRAY);
            DrawText("- ESC: Open pause menu", 120, 550, 25, DARKGRAY);

            // Botón VOLVER (siempre seleccionado)
            DrawRectangle(SCREEN_WIDTH / 2 - 100, 650, 200, 60, BUTTON_SELECTED);
            DrawText("BACK (ENTER/ESC)", SCREEN_WIDTH / 2 - MeasureText("BACK (ENTER/ESC)", 20) / 2,
                     665, 20, WHITE);
            break;
        }

        EndDrawing();
        //---------------------------------------------------------------------
    }

    CloseWindow();
    return 0;
}