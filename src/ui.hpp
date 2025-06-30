#pragma once
#include "game.hpp"

// Funciones de UI
void UpdateMenu();
void UpdateGameScreen();
void UpdatePauseScreen();
void UpdateControlsScreen();
void UpdateVictoryScreen();

void DrawMenu();
void DrawGameScreen();
void DrawPauseScreen();
void DrawControlsScreen();
void DrawVictoryScreen();

// Variables de UI
extern int mazeOffsetX;
extern int mazeOffsetY;