#pragma once
#include <vector>
#include <utility>

using pii = std::pair<int,int>;

// Algoritmo modular Dijkstra que recibe:
//   - map: laberinto
//   - start, end: posiciones de inicio/fin
//   - maxEnergy: pasos necesarios para romper
//   - startEnergy: energía inicial del jugador
//   - startBroken: si ya rompió un muro
std::vector<pii> dijkstraAlgorithm(
    const std::vector<std::vector<char>>& map,
    pii start,
    pii end,
    int maxEnergy,
    int startEnergy,
    bool startBroken
);

// Función pública para tu juego: arranca con energia y estado actuales
std::vector<pii> FindPath(
    pii start,
    pii end,
    int startEnergy = 0,
    bool startBroken = false
);
