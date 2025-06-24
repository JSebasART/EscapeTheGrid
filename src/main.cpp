#include "raylib.h"
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <utility>

// Constantes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 860;
const int CELL_SIZE = 80;
const int UI_HEIGHT = 60;
const Color PLAYER_COLOR = BLUE;
const Color WALL_COLOR = DARKGRAY;
const Color PATH_COLOR = LIGHTGRAY;
const Color START_COLOR = GREEN;
const Color END_COLOR = RED;
const Color ITEM_COLOR = ORANGE;
const Color BUTTON_COLOR = LIGHTGRAY;
const Color BUTTON_SELECTED = SKYBLUE;
const Color VICTORY_COLOR = Color{46, 204, 113, 255};
const Color ENERGY_BAR_COLOR = DARKGRAY;
const Color SOLUTION_COLOR = Color{255, 107, 107, 255}; // Rojo claro para solución

enum GameScreen
{
    MENU,
    GAME,
    PAUSE,
    CONTROLS,
    VICTORY
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

// Laberinto original para restablecer
std::vector<std::vector<char>> originalMaze = maze;

// Definir el tipo para los tamaños
typedef std::vector<std::vector<char>>::size_type size_type;

struct Player
{
    int row;
    int col;
};

// Función para encontrar el camino más corto usando BFS
std::vector<std::pair<int, int>> FindPath(const std::vector<std::vector<char>>& grid, 
                                          std::pair<int, int> start, 
                                          std::pair<int, int> end) {
    // Direcciones: arriba, derecha, abajo, izquierda
    const int dr[] = {-1, 0, 1, 0};
    const int dc[] = {0, 1, 0, -1};
    
    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());
    
    // Matriz para rastrear celdas visitadas
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    // Mapa para rastrear el camino
    std::vector<std::vector<std::pair<int, int>>> parent(rows, std::vector<std::pair<int, int>>(cols, {-1, -1}));
    
    std::queue<std::pair<int, int>> q;
    q.push(start);
    visited[start.first][start.second] = true;
    
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        
        // Si llegamos al final, reconstruimos el camino
        if (current == end) {
            std::vector<std::pair<int, int>> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current.first][current.second];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Explorar vecinos
        for (int i = 0; i < 4; i++) {
            int newRow = current.first + dr[i];
            int newCol = current.second + dc[i];
            
            // Verificar límites y si es transitable
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols &&
                !visited[newRow][newCol] && grid[newRow][newCol] != '#') {
                visited[newRow][newCol] = true;
                parent[newRow][newCol] = current;
                q.push({newRow, newCol});
            }
        }
    }
    
    // Si no se encontró camino, devolver vacío
    return {};
}

int main()
{
    // Inicializar ventana
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "La Casa de los Trucos");
    SetExitKey(0); // Deshabilita ESC como tecla de salida
    SetTargetFPS(60);

    // Estado del juego
    GameScreen currentScreen = MENU;
    Player player = {0, 0};           // Posición inicial (S)
    int selectedOption = 0;           // Opción seleccionada en menús
    int selectedWinOption = 0;        // Opción seleccionada en pantalla de victoria
    GameScreen previousScreen = MENU; // Pantalla anterior para volver de CONTROLS
    int stepCount = 0;                // Contador de pasos
    bool solvePath = false;           // Mostrar camino resuelto
    int itemsCollected = 0;           // Contador de coleccionables recolectados
    std::vector<std::pair<int, int>> solutionPath; // Camino solución

    // Encontrar posición inicial (S) y final (G)
    std::pair<int, int> startPos = {0, 0};
    std::pair<int, int> endPos = {0, 0};
    for (size_type i = 0; i < maze.size(); i++)
    {
        for (size_type j = 0; j < maze[i].size(); j++)
        {
            if (maze[i][j] == 'S') {
                player.row = static_cast<int>(i);
                player.col = static_cast<int>(j);
                startPos = {player.row, player.col};
            }
            if (maze[i][j] == 'G') {
                endPos = {static_cast<int>(i), static_cast<int>(j)};
            }
        }
    }

    // Coordenadas para centrar el laberinto (con espacio para UI)
    int mazeOffsetX = (SCREEN_WIDTH - (static_cast<int>(maze[0].size()) * CELL_SIZE)) / 2;
    int mazeOffsetY = UI_HEIGHT + (SCREEN_HEIGHT - UI_HEIGHT - (static_cast<int>(maze.size()) * CELL_SIZE)) / 2;

    // Bucle principal del juego
    while (!WindowShouldClose())
    {
        // Actualización del juego
        //---------------------------------------------------------------------
        switch (currentScreen)
        {
        case MENU: {
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
                    // Restaurar laberinto original para nuevo juego
                    maze = originalMaze;
                    // Resetear posición del jugador y contador de pasos
                    for (size_type i = 0; i < maze.size(); i++)
                    {
                        for (size_type j = 0; j < maze[i].size(); j++)
                        {
                            if (maze[i][j] == 'S')
                            {
                                player.row = static_cast<int>(i);
                                player.col = static_cast<int>(j);
                                startPos = {player.row, player.col};
                            }
                            if (maze[i][j] == 'G') {
                                endPos = {static_cast<int>(i), static_cast<int>(j)};
                            }
                        }
                    }
                    stepCount = 0;
                    solvePath = false;
                    itemsCollected = 0;
                    // Calcular nuevo camino solución
                    solutionPath = FindPath(maze, startPos, endPos);
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
        }
            
        case GAME: {
            // Movimiento del jugador
            bool moved = false;
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                if (player.row > 0 && maze[player.row - 1][player.col] != '#')
                {
                    player.row--;
                    stepCount++;
                    moved = true;
                }
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                if (player.row < static_cast<int>(maze.size()) - 1 &&
                    maze[player.row + 1][player.col] != '#')
                {
                    player.row++;
                    stepCount++;
                    moved = true;
                }
            }
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
            {
                if (player.col > 0 && maze[player.row][player.col - 1] != '#')
                {
                    player.col--;
                    stepCount++;
                    moved = true;
                }
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
            {
                if (player.col < static_cast<int>(maze[0].size()) - 1 &&
                    maze[player.row][player.col + 1] != '#')
                {
                    player.col++;
                    stepCount++;
                    moved = true;
                }
            }
            
            // Verificar si recogió un coleccionable
            if (moved && maze[player.row][player.col] == 'K') {
                // Reemplazar coleccionable con celda normal
                maze[player.row][player.col] = '.';
                itemsCollected++;
                // Recalcular camino solución
                solutionPath = FindPath(maze, {player.row, player.col}, endPos);
            }

            // Pausa con ESC (única función de ESC)
            if (IsKeyPressed(KEY_ESCAPE))
            {
                selectedOption = 0; // Resetear opción seleccionada
                currentScreen = PAUSE;
            }

            // Verificar si llegó a la meta
            if (maze[player.row][player.col] == 'G')
            {
                selectedWinOption = 0; // Resetear opción en pantalla de victoria
                currentScreen = VICTORY;
            }
            
            // Botón resolver con clic (toggle)
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                Rectangle solveButton = {SCREEN_WIDTH - 160, 10, 150, 40};
                if (CheckCollisionPointRec(mousePos, solveButton)) {
                    solvePath = !solvePath; // Alternar estado
                }
            }
            break;
        }
            
        case PAUSE: {
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
                    // No resetear posición aquí, se hará al volver a jugar
                    break;
                }
            }

            // Volver al juego con ESC
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = GAME;
            }
            break;
        }
            
        case CONTROLS: {
            // Solo hay una opción (VOLVER), seleccionada por defecto
            selectedOption = 0;

            // Selección con ENTER
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = previousScreen;
            }
            break;
        }
            
        case VICTORY: {
            // Navegación con flechas o WASD
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || 
                IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                selectedWinOption = (selectedWinOption + 1) % 2;
            }

            // Selección con ENTER
            if (IsKeyPressed(KEY_ENTER))
            {
                if (selectedWinOption == 0) {
                    // Siguiente nivel (reiniciar con laberinto original)
                    maze = originalMaze;
                    // Resetear posición del jugador
                    for (size_type i = 0; i < maze.size(); i++)
                    {
                        for (size_type j = 0; j < maze[i].size(); j++)
                        {
                            if (maze[i][j] == 'S')
                            {
                                player.row = static_cast<int>(i);
                                player.col = static_cast<int>(j);
                                startPos = {player.row, player.col};
                            }
                            if (maze[i][j] == 'G') {
                                endPos = {static_cast<int>(i), static_cast<int>(j)};
                            }
                        }
                    }
                    stepCount = 0;
                    solvePath = false;
                    itemsCollected = 0;
                    // Calcular nuevo camino solución
                    solutionPath = FindPath(maze, startPos, endPos);
                    currentScreen = GAME;
                } else {
                    currentScreen = MENU;
                }
            }
            break;
        }
        }
        //---------------------------------------------------------------------

        // Dibujado
        //---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScreen)
        {
        case MENU: {
            // Título
            DrawText("LA CASA DE LOS TRUCOS", SCREEN_WIDTH / 2 - MeasureText("LA CASA DE LOS TRUCOS", 50) / 2, 150, 50, DARKGRAY);

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

            break;
        }
            
        case GAME: {
            // Barra de energía (rectángulo sin rellenar)
            DrawRectangleLines(10, 10, 50, 40, ENERGY_BAR_COLOR);
            // Contador de pasos (al lado del rectángulo)
            DrawText(("Pasos: " + std::to_string(stepCount)).c_str(), 70, 20, 20, DARKGRAY);
            
            // Botón Resolver
            DrawRectangle(SCREEN_WIDTH - 160, 10, 150, 40, 
                          solvePath ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("RESOLVER", SCREEN_WIDTH - 155, 20, 20, DARKGRAY);
            
            // Mostrar coleccionable en el centro superior
            DrawCircle(SCREEN_WIDTH / 2, 30, 15, ITEM_COLOR);
            DrawText(TextFormat("x%d", itemsCollected), SCREEN_WIDTH / 2 + 20, 20, 20, DARKGRAY);

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
                    
                    // Dibujar coleccionable como bola naranja en celdas normales
                    if (maze[i][j] == 'K') {
                        DrawCircle(
                            mazeOffsetX + j * CELL_SIZE + CELL_SIZE / 2,
                            mazeOffsetY + i * CELL_SIZE + CELL_SIZE / 2,
                            CELL_SIZE / 4,
                            ITEM_COLOR
                        );
                    }
                }
            }

            // Dibujar camino solución si está activo
            if (solvePath && !solutionPath.empty()) {
                for (const auto& point : solutionPath) {
                    int i = point.first;
                    int j = point.second;
                    // Solo dibujar en celdas que no sean paredes
                    if (maze[i][j] != '#') {
                        DrawCircle(
                            mazeOffsetX + j * CELL_SIZE + CELL_SIZE / 2,
                            mazeOffsetY + i * CELL_SIZE + CELL_SIZE / 2,
                            CELL_SIZE / 8,
                            SOLUTION_COLOR
                        );
                    }
                }
            }

            // Dibujar jugador
            DrawCircle(
                mazeOffsetX + player.col * CELL_SIZE + CELL_SIZE / 2,
                mazeOffsetY + player.row * CELL_SIZE + CELL_SIZE / 2,
                CELL_SIZE / 3,
                PLAYER_COLOR);
            break;
        }
            
        case PAUSE: {
            // Fondo semitransparente
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));

            // Panel de pausa
            DrawRectangle(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 150, 300, 300, LIGHTGRAY);
            DrawText("PAUSA", SCREEN_WIDTH / 2 - MeasureText("PAUSA", 40) / 2, SCREEN_HEIGHT / 2 - 130, 40, DARKGRAY);

            // Botón CONTINUE
            DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 60, 200, 50,
                          selectedOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("CONTINUAR", SCREEN_WIDTH / 2 - MeasureText("CONTINUAR", 20) / 2,
                     SCREEN_HEIGHT / 2 - 45, 20, selectedOption == 0 ? WHITE : DARKGRAY);

            // Botón CONTROLS
            DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 50,
                          selectedOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("CONTROLES", SCREEN_WIDTH / 2 - MeasureText("CONTROLES", 20) / 2,
                     SCREEN_HEIGHT / 2 + 15, 20, selectedOption == 1 ? WHITE : DARKGRAY);

            // Botón EXIT TO MENU
            DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 200, 50,
                          selectedOption == 2 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("MENU PRINCIPAL", SCREEN_WIDTH / 2 - MeasureText("MENU PRINCIPAL", 20) / 2,
                     SCREEN_HEIGHT / 2 + 75, 20, selectedOption == 2 ? WHITE : DARKGRAY);

            break;
        }
            
        case CONTROLS: {
            // Fondo
            ClearBackground(RAYWHITE);

            // Título
            DrawText("CONTROLES", SCREEN_WIDTH / 2 - MeasureText("CONTROLES", 50) / 2, 100, 50, DARKGRAY);

            // Instrucciones
            DrawText("MOVIMIENTO DEL JUGADOR:", 100, 200, 30, DARKGRAY);
            DrawText("- Flechas: Arriba, Abajo, Izquierda, Derecha", 120, 250, 25, DARKGRAY);
            DrawText("- WASD: Alternativa para movimiento", 120, 290, 25, DARKGRAY);

            DrawText("NAVEGACIÓN DE MENÚ:", 100, 350, 30, DARKGRAY);
            DrawText("- Flechas/WASD: Mover entre opciones", 120, 400, 25, DARKGRAY);
            DrawText("- ENTER: Seleccionar opción", 120, 440, 25, DARKGRAY);

            DrawText("CONTROLES DE JUEGO:", 100, 500, 30, DARKGRAY);
            DrawText("- ESC: Abrir menú de pausa", 120, 550, 25, DARKGRAY);
            DrawText("- Click en RESOLVER: Mostrar camino solución", 120, 590, 25, DARKGRAY);

            // Botón VOLVER (siempre seleccionado)
            DrawRectangle(SCREEN_WIDTH / 2 - 100, 650, 200, 60, BUTTON_SELECTED);
            DrawText("VOLVER", SCREEN_WIDTH / 2 - MeasureText("VOLVER", 20) / 2,
                     665, 20, WHITE);
            break;
        }
            
        case VICTORY: {
            // Fondo de victoria
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(VICTORY_COLOR, 0.9f));
            
            // Mensaje de victoria
            DrawText("¡GANASTE!", SCREEN_WIDTH / 2 - MeasureText("¡GANASTE!", 70) / 2, 
                     SCREEN_HEIGHT / 2 - 150, 70, WHITE);
            
            // Mensaje adicional si se recolectó el coleccionable
            if (itemsCollected > 0) {
                DrawText(TextFormat("¡Recolectaste %d coleccionable(s)!", itemsCollected),
                         SCREEN_WIDTH / 2 - MeasureText(TextFormat("¡Recolectaste %d coleccionable(s)!", itemsCollected), 30) / 2,
                         SCREEN_HEIGHT / 2 - 80, 30, WHITE);
            }
            
            // Botón SIGUIENTE NIVEL
            DrawRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 400, 60,
                          selectedWinOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("SIGUIENTE NIVEL", SCREEN_WIDTH / 2 - MeasureText("SIGUIENTE NIVEL", 30) / 2, 
                     SCREEN_HEIGHT / 2 + 15, 30, selectedWinOption == 0 ? WHITE : DARKGRAY);
            
            // Botón MENÚ PRINCIPAL
            DrawRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 100, 400, 60,
                          selectedWinOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
            DrawText("MENÚ PRINCIPAL", SCREEN_WIDTH / 2 - MeasureText("MENÚ PRINCIPAL", 30) / 2, 
                     SCREEN_HEIGHT / 2 + 115, 30, selectedWinOption == 1 ? WHITE : DARKGRAY);
            
            break;
        }
        }

        EndDrawing();
        //---------------------------------------------------------------------
    }

    CloseWindow();
    return 0;
}