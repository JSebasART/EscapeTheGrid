#include "ui.hpp"
#include "game.hpp"
#include "maze.hpp"
#include "pathfinding.hpp"
#include <string>

int mazeOffsetX = 0;
int mazeOffsetY = 0;

// Implementación de funciones de actualización
void UpdateMenu() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % 3;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption + 2) % 3;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        switch (selectedOption) {
            case 0:
                ResetGame();
                currentScreen = GAME;
                break;
            case 1:
                previousScreen = MENU;
                currentScreen = CONTROLS;
                break;
            case 2:
                CloseWindow();
                exit(0);
        }
    }
}

void UpdateGameScreen() {
    // Movimiento del jugador
    bool moved = false;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        lastDirection = KEY_UP;
        if (player.row > 0 && maze[player.row - 1][player.col] != '#') {
            player.row--;
            stepCount++;
            moved = true;
            energy += 10.0f;
            if (energy > 100.0f) energy = 100.0f;
        }
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        lastDirection = KEY_DOWN;
        if (player.row < static_cast<int>(maze.size()) - 1 &&
            maze[player.row + 1][player.col] != '#') {
            player.row++;
            stepCount++;
            moved = true;
            energy += 10.0f;
            if (energy > 100.0f) energy = 100.0f;
        }
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        lastDirection = KEY_LEFT;
        if (player.col > 0 && maze[player.row][player.col - 1] != '#') {
            player.col--;
            stepCount++;
            moved = true;
            energy += 10.0f;
            if (energy > 100.0f) energy = 100.0f;
        }
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        lastDirection = KEY_RIGHT;
        if (player.col < static_cast<int>(maze[0].size()) - 1 &&
            maze[player.row][player.col + 1] != '#') {
            player.col++;
            stepCount++;
            moved = true;
            energy += 10.0f;
            if (energy > 100.0f) energy = 100.0f;
        }
    }

    // Actualizar animación del jugador
    animationTimer += GetFrameTime();
    if (animationTimer >= FRAME_TIME) {
        animationTimer = 0.0f;
        playerFrame = (playerFrame + 1) % 2; // Alternar entre 2 frames
    }
    
    // Verificar coleccionable
    if (moved && maze[player.row][player.col] == 'K') {
        maze[player.row][player.col] = '.';
        itemsCollected++;
        solutionPath = FindPath(FindEndPosition());
    }

    // Romper pared
    if (energy >= 100.0f && IsKeyPressed(KEY_SPACE)) {
        int targetRow = player.row;
        int targetCol = player.col;
        
        switch (lastDirection) {
            case KEY_UP: targetRow--; break;
            case KEY_DOWN: targetRow++; break;
            case KEY_LEFT: targetCol--; break;
            case KEY_RIGHT: targetCol++; break;
        }
        
        if (targetRow >= 0 && targetRow < static_cast<int>(maze.size()) &&
            targetCol >= 0 && targetCol < static_cast<int>(maze[0].size()) &&
            maze[targetRow][targetCol] == '#') {
            
            maze[targetRow][targetCol] = '.';
            energy = 0.0f;
            solutionPath = FindPath(FindEndPosition());
        }
    }

    // Actualizar estado de energía
    canBreakWall = (energy >= 100.0f);

    // Pausa
    if (IsKeyPressed(KEY_ESCAPE)) {
        selectedOption = 0;
        currentScreen = PAUSE;
    }

    // Victoria
    if (maze[player.row][player.col] == 'G') {
        selectedWinOption = 0;
        currentScreen = VICTORY;
    }
    
    // Botón resolver
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle solveButton = {SCREEN_WIDTH - 160, 10, 150, 40};
        if (CheckCollisionPointRec(mousePos, solveButton)) {
            solvePath = !solvePath;
        }
    }
}

void UpdatePauseScreen() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % 3;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption + 2) % 3;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        switch (selectedOption) {
            case 0: currentScreen = GAME; break;
            case 1: 
                previousScreen = PAUSE;
                currentScreen = CONTROLS;
                break;
            case 2: currentScreen = MENU; break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        currentScreen = GAME;
    }
}

void UpdateControlsScreen() {
    selectedOption = 0;

    if (IsKeyPressed(KEY_ENTER)) {
        currentScreen = previousScreen;
    }
}

void UpdateVictoryScreen() {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || 
        IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedWinOption = (selectedWinOption + 1) % 2;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedWinOption == 0) {
            // Reiniciar el mismo mapa
            ResetGame();
            currentScreen = GAME;
        } else {
            currentScreen = MENU;
        }
    }
}

// Implementación de funciones de dibujo
void DrawMenu() {
    DrawText("LA CASA DE LOS TRUCOS", SCREEN_WIDTH / 2 - MeasureText("LA CASA DE LOS TRUCOS", 50) / 2, 150, 50, DARKGRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - 100, 300, 200, 60, selectedOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("PLAY", SCREEN_WIDTH / 2 - MeasureText("PLAY", 30) / 2, 315, 30, selectedOption == 0 ? WHITE : DARKGRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - 100, 380, 200, 60, selectedOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("CONTROLS", SCREEN_WIDTH / 2 - MeasureText("CONTROLS", 30) / 2, 395, 30, selectedOption == 1 ? WHITE : DARKGRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - 100, 460, 200, 60, selectedOption == 2 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("EXIT", SCREEN_WIDTH / 2 - MeasureText("EXIT", 30) / 2, 475, 30, selectedOption == 2 ? WHITE : DARKGRAY);
}

void DrawGameScreen() {
    // Barra de energía
    DrawRectangle(10, 10, 50, 40, LIGHTGRAY);
    DrawRectangle(10, 10, static_cast<int>(50 * (energy/100.0f)), 40, ENERGY_FILL_COLOR);
    DrawRectangleLines(10, 10, 50, 40, DARKGRAY);
    if (canBreakWall) DrawRectangleLines(10, 10, 50, 40, ENERGY_READY_COLOR);
    DrawText("Energia", 10, 55, 15, DARKGRAY);

    // Contador de pasos
    DrawText(("Pasos: " + std::to_string(stepCount)).c_str(), 70, 20, 20, DARKGRAY);
    
    // Botón resolver
    DrawRectangle(SCREEN_WIDTH - 160, 10, 150, 40, solvePath ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("RESOLVER", SCREEN_WIDTH - 155, 20, 20, DARKGRAY);
    
    // Coleccionables
    DrawCircle(SCREEN_WIDTH / 2, 30, 15, ITEM_COLOR);
    DrawText(TextFormat("x%d", itemsCollected), SCREEN_WIDTH / 2 + 20, 20, 20, DARKGRAY);

    // Calcular offsets si no se han calculado
    if (mazeOffsetX == 0 && mazeOffsetY == 0) {
        mazeOffsetX = (SCREEN_WIDTH - (static_cast<int>(maze[0].size()) * CELL_SIZE)) / 2;
        mazeOffsetY = UI_HEIGHT + (SCREEN_HEIGHT - UI_HEIGHT - (static_cast<int>(maze.size()) * CELL_SIZE)) / 2;
    }

    // Dibujar laberinto
    for (size_t i = 0; i < maze.size(); i++) {
        for (size_t j = 0; j < maze[i].size(); j++) {
            Color cellColor;
            switch (maze[i][j]) {
                case '#': cellColor = WALL_COLOR; break;
                case 'S': cellColor = START_COLOR; break;
                case 'G': cellColor = END_COLOR; break;
                default: cellColor = PATH_COLOR;
            }

            DrawRectangle(
                mazeOffsetX + static_cast<int>(j) * CELL_SIZE,
                mazeOffsetY + static_cast<int>(i) * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE,
                cellColor);

            DrawRectangleLines(
                mazeOffsetX + static_cast<int>(j) * CELL_SIZE,
                mazeOffsetY + static_cast<int>(i) * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE,
                BLACK);
            
            // Coleccionable
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

    // Camino solución
    if (solvePath && !solutionPath.empty()) {
        for (const auto& point : solutionPath) {
            int i = point.first;
            int j = point.second;
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

    // Dibujar jugador con sprite sheet
    if (playerTexture.id > 0) { // Solo si la textura es válida
        // Calcular tamaño de frame CORREGIDO (usar int para evitar decimales)
        int frameWidth = playerTexture.width / 2;
        int frameHeight = playerTexture.height;
        
        Rectangle sourceRec = {
            static_cast<float>(playerFrame * frameWidth),
            0.0f,
            static_cast<float>(frameWidth),
            static_cast<float>(frameHeight)
        };
        
        Rectangle destRec = {
            static_cast<float>(mazeOffsetX + player.col * CELL_SIZE),
            static_cast<float>(mazeOffsetY + player.row * CELL_SIZE),
            static_cast<float>(CELL_SIZE),
            static_cast<float>(CELL_SIZE)
        };
        
        DrawTexturePro(playerTexture, sourceRec, destRec, Vector2{0,0}, 0.0f, WHITE);
    }
    else {
        // Fallback: dibujar cuadrado si no hay textura
        DrawRectangle(
            mazeOffsetX + player.col * CELL_SIZE,
            mazeOffsetY + player.row * CELL_SIZE,
            CELL_SIZE, CELL_SIZE, PLAYER_COLOR);
    }
}

void DrawPauseScreen() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));
    DrawRectangle(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 150, 300, 300, LIGHTGRAY);
    DrawText("PAUSA", SCREEN_WIDTH / 2 - MeasureText("PAUSA", 40) / 2, SCREEN_HEIGHT / 2 - 130, 40, DARKGRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 60, 200, 50, selectedOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("CONTINUAR", SCREEN_WIDTH / 2 - MeasureText("CONTINUAR", 20) / 2, SCREEN_HEIGHT / 2 - 45, 20, selectedOption == 0 ? WHITE : DARKGRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 50, selectedOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("CONTROLES", SCREEN_WIDTH / 2 - MeasureText("CONTROLES", 20) / 2, SCREEN_HEIGHT / 2 + 15, 20, selectedOption == 1 ? WHITE : DARKGRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 200, 50, selectedOption == 2 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("MENU PRINCIPAL", SCREEN_WIDTH / 2 - MeasureText("MENU PRINCIPAL", 20) / 2, SCREEN_HEIGHT / 2 + 75, 20, selectedOption == 2 ? WHITE : DARKGRAY);
}

void DrawControlsScreen() {
    ClearBackground(RAYWHITE);
    DrawText("CONTROLES", SCREEN_WIDTH / 2 - MeasureText("CONTROLES", 50) / 2, 100, 50, DARKGRAY);

    DrawText("MOVIMIENTO DEL JUGADOR:", 100, 200, 30, DARKGRAY);
    DrawText("- Flechas: Arriba, Abajo, Izquierda, Derecha", 120, 250, 25, DARKGRAY);
    DrawText("- WASD: Alternativa para movimiento", 120, 290, 25, DARKGRAY);
    
    DrawText("ROMPER PAREDES:", 100, 340, 30, DARKGRAY);
    DrawText("- Espacio: Romper pared (requiere energía completa)", 120, 390, 25, DARKGRAY);
    DrawText("- La energía se carga al moverse", 120, 430, 25, DARKGRAY);

    DrawText("NAVEGACIÓN DE MENÚ:", 100, 480, 30, DARKGRAY);
    DrawText("- Flechas/WASD: Mover entre opciones", 120, 530, 25, DARKGRAY);
    DrawText("- ENTER: Seleccionar opción", 120, 570, 25, DARKGRAY);

    DrawText("CONTROLES DE JUEGO:", 100, 620, 30, DARKGRAY);
    DrawText("- ESC: Abrir menú de pausa", 120, 670, 25, DARKGRAY);
    DrawText("- Click en RESOLVER: Mostrar camino solución", 120, 710, 25, DARKGRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - 100, 750, 200, 60, BUTTON_SELECTED);
    DrawText("VOLVER", SCREEN_WIDTH / 2 - MeasureText("VOLVER", 20) / 2, 765, 20, WHITE);
}

void DrawVictoryScreen() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(VICTORY_COLOR, 0.9f));
    DrawText("¡GANASTE!", SCREEN_WIDTH / 2 - MeasureText("¡GANASTE!", 70) / 2, SCREEN_HEIGHT / 2 - 150, 70, WHITE);
    
    if (itemsCollected > 0) {
        DrawText(TextFormat("¡Recolectaste %d coleccionable(s)!", itemsCollected),
                 SCREEN_WIDTH / 2 - MeasureText(TextFormat("¡Recolectaste %d coleccionable(s)!", itemsCollected), 30) / 2,
                 SCREEN_HEIGHT / 2 - 80, 30, WHITE);
    }
    
    // Cambiar texto a "JUGAR DE NUEVO"
    DrawRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 400, 60, selectedWinOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("JUGAR DE NUEVO", SCREEN_WIDTH / 2 - MeasureText("JUGAR DE NUEVO", 30) / 2, SCREEN_HEIGHT / 2 + 15, 30, selectedWinOption == 0 ? WHITE : DARKGRAY);
    
    DrawRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 100, 400, 60, selectedWinOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("MENÚ PRINCIPAL", SCREEN_WIDTH / 2 - MeasureText("MENÚ PRINCIPAL", 30) / 2, SCREEN_HEIGHT / 2 + 115, 30, selectedWinOption == 1 ? WHITE : DARKGRAY);
}