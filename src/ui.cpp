#include "ui.hpp"
#include "game.hpp"
#include "maze.hpp"
#include "pathFinding.hpp"
#include <string>
#include <cmath>

int mazeOffsetX = 0;
int mazeOffsetY = 0;
static const float AUTO_MOVE_INTERVAL = 0.5f;
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
    bool moved = false;
     float dt = GetFrameTime();
    autoMoveTimer += dt;
    // Movimiento manual
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        lastDirection = KEY_UP;
        if (player.row > 0 && maze[player.row - 1][player.col] != '#') {
            player.row--; moved = true; stepCount++; energy = std::min(energy+10.0f,100.0f); stepsSinceLastChange++;
        }
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        lastDirection = KEY_DOWN;
        if (player.row < (int)maze.size()-1 && maze[player.row+1][player.col] != '#') {
            player.row++; moved = true; stepCount++; energy = std::min(energy+10.0f,100.0f); stepsSinceLastChange++;
        }
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        lastDirection = KEY_LEFT;
        if (player.col > 0 && maze[player.row][player.col-1] != '#') {
            player.col--; moved = true; stepCount++; energy = std::min(energy+10.0f,100.0f); stepsSinceLastChange++;
        }
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        lastDirection = KEY_RIGHT;
        if (player.col < (int)maze[0].size()-1 && maze[player.row][player.col+1] != '#') {
            player.col++; moved = true; stepCount++; energy = std::min(energy+10.0f,100.0f); stepsSinceLastChange++;
        }
    }
     if (moved) {
        MoveDynamicWalls(2, player.row, player.col);
    }
    
     if (isAutoSolving && autoMoveTimer >= AUTO_MOVE_INTERVAL) {
        autoMoveTimer = 0.0f;  // reseteamos el contador

        // 1) Celda destino
        auto [tr, tc] = solutionPath[autoStepIndex];

        // 2) Si un muro bloquea, recalculamos o abortamos
        if (maze[tr][tc] == '#') {
            auto newPath = FindPath(
                {player.row, player.col},
                FindEndPosition(),
                std::min(MAX_ENERGY_STEPS, int(energy * MAX_ENERGY_STEPS/100.0f)),
                hasBrokenWall
            );
            if (newPath.empty()) {
                isAutoSolving = false;
                noSolution    = true;
            } else {
                solutionPath  = newPath;
                autoStepIndex = 0;
            }
        }
        // 3) Si está libre, avanzamos y movemos muros
        else if (autoStepIndex < (int)solutionPath.size()) {
            // a) mover al jugador
            player.row = tr;
            player.col = tc;
            stepCount++;
            energy = std::min(energy + 10.0f, 100.0f);
            autoStepIndex++;
            stepsSinceLastChange++;

            // b) desplazar 2 muros dinámicos
            MoveDynamicWalls(2, player.row, player.col);
        }
    }
    // Desaparición única de paredes
    if (!wallsRemoved && stepsSinceLastChange >= DISAPPEAR_INTERVAL) {
        RemoveRandomWalls(WALLS_TO_REMOVE);
        wallsRemoved = true;
        stepsSinceLastChange = 0;
    }
    // Interacción con ítem
    if (moved && maze[player.row][player.col] == 'K') {
        maze[player.row][player.col] = '.';
        itemsCollected++;
        int units = std::min(MAX_ENERGY_STEPS, (int)(energy*MAX_ENERGY_STEPS/100.0f));
        solutionPath = FindPath({player.row,player.col}, FindEndPosition(), units, hasBrokenWall);
    }
    // Romper pared manual (una sola vez)
    if (energy >= 100.0f && !hasBrokenWall && IsKeyPressed(KEY_SPACE)) {
        int tr=player.row, tc=player.col;
        switch(lastDirection) {
            case KEY_UP: tr--; break;
            case KEY_DOWN: tr++; break;
            case KEY_LEFT: tc--; break;
            case KEY_RIGHT: tc++; break;
        }
        if (maze[tr][tc]=='#') BreakWallAt(tr,tc);
        canBreakWall = false;
    }
    // Toggle autoplay
    if (IsKeyPressed(KEY_C)) {
        if (!isAutoSolving) {
            solvePath=true; isAutoSolving=true; autoStepIndex=0; autoMoveTimer=0.0f;
            int units = std::min(MAX_ENERGY_STEPS, (int)(energy*MAX_ENERGY_STEPS/100.0f));
            solutionPath = FindPath({player.row,player.col}, FindEndPosition(), units, hasBrokenWall);
        } else {
            isAutoSolving=false;
        }
    }
    // Autoplay 1 paso/s
    if (isAutoSolving && autoStepIndex < (int)solutionPath.size()) {
        autoMoveTimer += GetFrameTime();
        if (autoMoveTimer >= 0.5f) {
            autoMoveTimer=0.0f;
            int tr=solutionPath[autoStepIndex].first;
            int tc=solutionPath[autoStepIndex].second;
            if (player.row!=tr || player.col!=tc) {
                int dr=tr-player.row, dc=tc-player.col;
                int nr=player.row+(dr>0?1:(dr<0?-1:0));
                int nc=player.col+(dc>0?1:(dc<0?-1:0));
                if (maze[nr][nc]=='#' && energy>=100.0f && !hasBrokenWall) {
                    BreakWallAt(nr,nc); stepCount++;
                } else if (maze[nr][nc] != '#') {
                    player.row=nr; player.col=nc; stepCount++; energy=std::min(energy+10.0f,100.0f);
                    autoStepIndex++; stepsSinceLastChange++;
                }
            } else {
                autoStepIndex++;
            }
            if (maze[player.row][player.col]=='G') isAutoSolving=false;
            if (!wallsRemoved && stepsSinceLastChange>=DISAPPEAR_INTERVAL) {
                RemoveRandomWalls(WALLS_TO_REMOVE);
                wallsRemoved=true; stepsSinceLastChange=0;
            }
        }
    }
    // Pausa
    if (IsKeyPressed(KEY_ESCAPE)) {
        selectedOption=0; currentScreen=PAUSE;
    }

    // Victoria
    if (maze[player.row][player.col]=='G') {
        selectedWinOption=0; currentScreen=VICTORY;
    }
}

void UpdatePauseScreen() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) selectedOption=(selectedOption+1)%3;
    if (IsKeyPressed(KEY_UP)   || IsKeyPressed(KEY_W)) selectedOption=(selectedOption+2)%3;
    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedOption==0) currentScreen=GAME;
        else if (selectedOption==1) { previousScreen=PAUSE; currentScreen=CONTROLS; }
        else currentScreen=MENU;
    }
    if (IsKeyPressed(KEY_ESCAPE)) currentScreen=GAME;
}

void UpdateControlsScreen() {
    selectedOption=0;
    if (IsKeyPressed(KEY_ENTER)) currentScreen=previousScreen;
}

void UpdateVictoryScreen() {
    if (IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_W)||IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_S)) {
        selectedWinOption=(selectedWinOption+1)%2;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedWinOption==0) { ResetGame(); currentScreen=GAME; }
        else currentScreen=MENU;
    }
}

// Implementación de funciones de dibujo
void DrawMenu() {
    // Dibujar fondo de menú
    if (menuBackgroundTexture.id != 0) {
        // Calcular para cubrir toda la pantalla manteniendo relación de aspecto
        float scaleX = (float)SCREEN_WIDTH / menuBackgroundTexture.width;
        float scaleY = (float)SCREEN_HEIGHT / menuBackgroundTexture.height;
        float scale = fmaxf(scaleX, scaleY); // Tomar la escala más grande
        
        // Calcular posición para centrar
        float scaledWidth = menuBackgroundTexture.width * scale;
        float scaledHeight = menuBackgroundTexture.height * scale;
        float offsetX = (SCREEN_WIDTH - scaledWidth) * 0.5f;
        float offsetY = (SCREEN_HEIGHT - scaledHeight) * 0.5f;
        
        // Dibujar textura cubriendo toda la pantalla
        DrawTextureEx(
            menuBackgroundTexture,
            Vector2{offsetX, offsetY},
            0.0f,
            scale,
            WHITE
        );
    } else {
        // Fallback: fondo de color sólido
        ClearBackground(RAYWHITE);
    }
    
    // Dibujar capa semi-transparente para mejorar legibilidad
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.3f));
    
    // Título con sombra para mejor contraste
    const char* title = "LA CASA DE LOS TRUCOS";
    int titleWidth = MeasureText(title, 50);
    
    // Sombra
    DrawText(title, SCREEN_WIDTH/2 - titleWidth/2 + 3, 153, 50, BLACK);
    // Texto principal
    DrawText(title, SCREEN_WIDTH/2 - titleWidth/2, 150, 50, RAYWHITE); // Dorado
    
    // Botones
    DrawRectangle(SCREEN_WIDTH / 2 - 100, 300, 200, 60, selectedOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("JUGAR", SCREEN_WIDTH / 2 - MeasureText("JUGAR", 30) / 2, 315, 30, selectedOption == 0 ? WHITE : DARKGRAY);
    
    DrawRectangle(SCREEN_WIDTH / 2 - 100, 380, 200, 60, selectedOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("CONTROLES", SCREEN_WIDTH / 2 - MeasureText("CONTROLES", 30) / 2, 395, 30, selectedOption == 1 ? WHITE : DARKGRAY);
    
    DrawRectangle(SCREEN_WIDTH / 2 - 100, 460, 200, 60, selectedOption == 2 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("SALIR", SCREEN_WIDTH / 2 - MeasureText("SALIR", 30) / 2, 475, 30, selectedOption == 2 ? WHITE : DARKGRAY);
}

void DrawGameScreen() {
    DrawRectangle(10, 10, 50, 40, LIGHTGRAY);
    DrawRectangleLines(10, 10, 50, 40, DARKGRAY);
    if (canBreakWall) {
        DrawRectangle(10, 10,
            static_cast<int>(50 * (energy/100.0f)),
            40,
            ENERGY_FILL_COLOR
        );
        DrawRectangleLines(10, 10, 50, 40, ENERGY_READY_COLOR);
    }

    // Contador de pasos
    DrawText(("PASOS: " + std::to_string(stepCount)).c_str(), 70, 20, 20, DARKGRAY);
    
    // Botón resolver
    DrawRectangle(SCREEN_WIDTH - 160, 10, 150, 40, solvePath ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("RESOLVER ¨R¨", SCREEN_WIDTH - 155, 20, 20, DARKGRAY);
    
    const char* resolveText = "RESOLVER";
    int textWidth = MeasureText(resolveText, 20); 
    DrawRectangle(SCREEN_WIDTH - 160, 10, 150, 40, solvePath ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText(
        resolveText, 
        (SCREEN_WIDTH - 160) + (150 - textWidth) / 2,  
        20,                                            
        20, 
        RAYWHITE
    );
    
    // Coleccionables
    Rectangle itemUIRect = {
        static_cast<float>(SCREEN_WIDTH / 2 - 15),
        15.0f,
        30.0f,
        30.0f
    };

    DrawTexturePro(
        currentThemeAssets.item,
        (Rectangle){0, 0, (float)currentThemeAssets.item.width, (float)currentThemeAssets.item.height},
        itemUIRect,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );

    DrawText(TextFormat("x%d", itemsCollected), SCREEN_WIDTH / 2 + 20, 20, 20, DARKGRAY);

    // Calcular offsets si no se han calculado
    if (mazeOffsetX == 0 && mazeOffsetY == 0) {
        mazeOffsetX = (SCREEN_WIDTH - (static_cast<int>(maze[0].size()) * CELL_SIZE)) / 2;
        mazeOffsetY = UI_HEIGHT + (SCREEN_HEIGHT - UI_HEIGHT - (static_cast<int>(maze.size()) * CELL_SIZE)) / 2;
    }

    // Dibujar laberinto con texturas temáticas
    for (size_t i = 0; i < maze.size(); i++) {
        for (size_t j = 0; j < maze[i].size(); j++) {
            // Rectángulo destino para esta celda
            Rectangle destRect = {
                static_cast<float>(mazeOffsetX + j * CELL_SIZE),
                static_cast<float>(mazeOffsetY + i * CELL_SIZE),
                static_cast<float>(CELL_SIZE),
                static_cast<float>(CELL_SIZE)
            };

            // 1. Dibujar fondo (camino) para todas las celdas
            DrawTexturePro(
                currentThemeAssets.path,
                (Rectangle){0, 0, (float)currentThemeAssets.path.width, (float)currentThemeAssets.path.height},
                destRect,
                (Vector2){0, 0},
                0.0f,
                WHITE
            );

            // 2. Dibujar elementos específicos sobre el fondo
            switch (maze[i][j]) {
                case '#': // Pared
                    DrawTexturePro(
                        currentThemeAssets.wall,
                        (Rectangle){0, 0, (float)currentThemeAssets.wall.width, (float)currentThemeAssets.wall.height},
                        destRect,
                        (Vector2){0, 0},
                        0.0f,
                        WHITE
                    );
                    break;
                    
                case 'S': // Inicio
                    DrawTexturePro(
                        currentThemeAssets.start,
                        (Rectangle){0, 0, (float)currentThemeAssets.start.width, (float)currentThemeAssets.start.height},
                        destRect,
                        (Vector2){0, 0},
                        0.0f,
                        WHITE
                    );
                    break;
                    
                case 'G': // Meta
                    DrawTexturePro(
                        currentThemeAssets.end,
                        (Rectangle){0, 0, (float)currentThemeAssets.end.width, (float)currentThemeAssets.end.height},
                        destRect,
                        (Vector2){0, 0},
                        0.0f,
                        WHITE
                    );
                    break;
                    
                case 'K': // Ítem
                    // Dibujar ítem centrado en la celda
                    DrawTexturePro(
                        currentThemeAssets.item,
                        (Rectangle){0, 0, (float)currentThemeAssets.item.width, (float)currentThemeAssets.item.height},
                        (Rectangle){
                            destRect.x + destRect.width/4,
                            destRect.y + destRect.height/4,
                            destRect.width/2,
                            destRect.height/2
                        },
                        (Vector2){0, 0},
                        0.0f,
                        WHITE
                    );
                    break;
            }
        }
    }

    // Dibujar camino solución
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

    Texture2D currentPlayerTexture = playerDown;

    switch (lastDirection) {
    case KEY_UP: currentPlayerTexture = playerUp; break;
    case KEY_DOWN: currentPlayerTexture = playerDown; break;
    case KEY_LEFT: currentPlayerTexture = playerLeft; break;
    case KEY_RIGHT: currentPlayerTexture = playerRight; break;
}

if (currentPlayerTexture.id > 0) {
        Rectangle sourceRec = {
            0.0f,
            0.0f,
            (float)currentPlayerTexture.width,
            (float)currentPlayerTexture.height
        };
        
        Rectangle destRec = {
            static_cast<float>(mazeOffsetX + player.col * CELL_SIZE),
            static_cast<float>(mazeOffsetY + player.row * CELL_SIZE),
            static_cast<float>(CELL_SIZE),
            static_cast<float>(CELL_SIZE)
        };
        
        if (noSolution) {
            const char* msg = "¡No hay solución disponible!";
            int w = MeasureText(msg, 20);
            DrawText(msg,
                    (SCREEN_WIDTH - w)/2,
                    SCREEN_HEIGHT - 40,
                    20,
                    RED);
        }

        DrawTexturePro(currentPlayerTexture, sourceRec, destRec, Vector2{0,0}, 0.0f, WHITE);
    }
    else {
        // Fallback: dibujar cuadrado
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
    // Dibujar la imagen de controles
    if (controlsTexture.id != 0) {
        // Calcular escala para que se ajuste al 90% del ancho de la pantalla
        float scale = static_cast<float>(SCREEN_WIDTH * 0.9) / controlsTexture.width;
        int scaledHeight = static_cast<int>(controlsTexture.height * scale);
        
        // Centrar verticalmente, dejando espacio para el botón
        int yPos = (SCREEN_HEIGHT - scaledHeight - 100) / 2;
        
        DrawTextureEx(
            controlsTexture,
            Vector2{
                static_cast<float>(SCREEN_WIDTH - controlsTexture.width * scale) / 2,
                static_cast<float>(yPos)
            },
            0.0f,
            scale,
            WHITE
        );
    } else {
        // Fallback: mostrar mensaje de error
        DrawText("ERROR: Imagen de controles no disponible", 50, 50, 30, RED);
    }
    
    // Botón VOLVER
    DrawRectangle(SCREEN_WIDTH / 2 - 100, 750, 200, 60, BUTTON_SELECTED);
    DrawText("REGRESAR", SCREEN_WIDTH / 2 - MeasureText("REGRESAR", 20) / 2, 765, 20, WHITE);
}

void DrawVictoryScreen() {
    // Dibujar imagen de fondo (si está cargada)
    if (victoryBgTexture.id != 0) {
        // Calcular escala para cubrir toda la pantalla manteniendo relación de aspecto
        float scaleX = (float)SCREEN_WIDTH / victoryBgTexture.width;
        float scaleY = (float)SCREEN_HEIGHT / victoryBgTexture.height;
        float scale = (scaleX > scaleY) ? scaleX : scaleY;
        
        int scaledWidth = victoryBgTexture.width * scale;
        int scaledHeight = victoryBgTexture.height * scale;
        int offsetX = (SCREEN_WIDTH - scaledWidth) / 2;
        int offsetY = (SCREEN_HEIGHT - scaledHeight) / 2;
        
        DrawTextureEx(
            victoryBgTexture,
            Vector2{(float)offsetX, (float)offsetY},
            0.0f,
            scale,
            WHITE
        );
    } else {
        // Fallback: fondo de color sólido
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, VICTORY_COLOR);
    }
    
    // Panel semitransparente para mejorar legibilidad del texto
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.3f));
    
    // Mensaje de victoria
    DrawText("¡LO LOGRASTE!", SCREEN_WIDTH / 2 - MeasureText("¡LO LOGRASTE!", 70) / 2, 
             SCREEN_HEIGHT / 2 - 150, 70, WHITE);
    
    // Mensaje adicional si se recolectó el coleccionable
    if (itemsCollected > 0) {
        DrawText(TextFormat("¡Recolectaste %d item!", itemsCollected),
                 SCREEN_WIDTH / 2 - MeasureText(TextFormat("¡Recolectaste %d item!", itemsCollected), 30) / 2,
                 SCREEN_HEIGHT / 2 - 80, 30, WHITE);    
    }
    
    // Botones con fondo semitransparente para mejor contraste
    DrawRectangle(SCREEN_WIDTH / 2 - 210, SCREEN_HEIGHT / 2 - 10, 420, 70, Fade(BLACK, 0.5f));
    DrawRectangle(SCREEN_WIDTH / 2 - 210, SCREEN_HEIGHT / 2 + 90, 420, 70, Fade(BLACK, 0.5f));
    
    // Botón SIGUIENTE NIVEL
    DrawRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 400, 60,
                  selectedWinOption == 0 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("JUGAR DE NUEVO", SCREEN_WIDTH / 2 - MeasureText("JUGAR DE NUEVO", 30) / 2, 
             SCREEN_HEIGHT / 2 + 15, 30, selectedWinOption == 0 ? WHITE : DARKGRAY);
    
    // Botón MENÚ PRINCIPAL
    DrawRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 100, 400, 60,
                  selectedWinOption == 1 ? BUTTON_SELECTED : BUTTON_COLOR);
    DrawText("MENÚ PRINCIPAL", SCREEN_WIDTH / 2 - MeasureText("MENÚ PRINCIPAL", 30) / 2, 
             SCREEN_HEIGHT / 2 + 115, 30, selectedWinOption == 1 ? WHITE : DARKGRAY);
}