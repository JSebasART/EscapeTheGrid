#include "pathFinding.hpp"
#include "maze.hpp"
#include <queue>
#include <algorithm>

struct State {
    int x, y;
    int cost;
    int energy;
    bool broken;
    std::vector<pii> path;
    bool operator>(const State& o) const { return cost > o.cost; }
};

std::vector<pii> dijkstraAlgorithm(
    const std::vector<std::vector<char>>& map,
    pii start,
    pii end,
    int maxEnergy,
    int startEnergy,
    bool startBroken
) {
    int n = map.size(), m = map[0].size();
    // visited[x][y][energy][broken]
    std::vector<std::vector<std::vector<std::vector<bool>>>> visited(
        n,
        std::vector<std::vector<std::vector<bool>>>(
            m,
            std::vector<std::vector<bool>>(maxEnergy+1, std::vector<bool>(2,false))
        )
    );

    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    // Estado inicial con energía y broken según el jugador
    pq.push({start.first, start.second, 0, startEnergy, startBroken, {start}});

    int dx[4] = {-1,1,0,0}, dy[4] = {0,0,-1,1};

    while (!pq.empty()) {
        State u = pq.top(); pq.pop();
        if (u.x == end.first && u.y == end.second) return u.path;
        if (visited[u.x][u.y][u.energy][u.broken]) continue;
        visited[u.x][u.y][u.energy][u.broken] = true;

        for (int i = 0; i < 4; i++) {
            int nx = u.x + dx[i], ny = u.y + dy[i];
            if (nx<0||ny<0||nx>=n||ny>=m) continue;
            char cell = map[nx][ny];

            State v = u;
            v.x = nx; v.y = ny;
            v.cost = u.cost + 1;
            v.path.push_back({nx, ny});

            // Paso libre o meta
            if (cell == '.' || cell == 'G') {
                v.energy = std::min(u.energy + 1, maxEnergy);
                pq.push(v);
            }
            // Muro — solo romper si energía completa y aún no rompió
            else if (cell == '#' && u.energy >= maxEnergy && !u.broken) {
                v.energy = 0;
                v.broken = true;
                pq.push(v);
            }
        }
    }
    return {};  // Sin camino
}

std::vector<pii> FindPath(
    pii start,
    pii end,
    int startEnergy,
    bool startBroken
) {
    const int MAX_ENERGY = 5;  // Parámetro de tu juego
    return dijkstraAlgorithm(maze, start, end, MAX_ENERGY, startEnergy, startBroken);
}
