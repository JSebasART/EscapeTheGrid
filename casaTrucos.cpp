#include <iostream> 
#include <fstream>    // Para leer el mapa del laberinto desde .txt)
#include <vector>     // Para usar vectores dinámicos (como matriz del laberinto)
#include <queue>      // Para usar una cola de prioridad (Dijkstra)
#include <tuple>      // Útil para pares/triples
#include <string>     // Para manipular strings las líneas o filas del laberinto
using namespace std;

// Estado que representa una posición en el laberinto
struct State {
    int x, y;           // Coordenadas del jugador en el laberinto
    int turn;
    int energy;         // Energía acumulada (incrementa con cada paso)
    bool brokenWall;    // ¿la pared ha sido rota? true o false
    int cost;           // Costo acumulado (cantidad de pasos hasta hasta el momento)
    vector<pair<int, int>> way;

    bool operator>(const State& other) const { // acomodar dikjistra segun ejemplo de clase
        return cost > other.cost;
    }
};

// Simula si una celda está disponible en el turn current
bool availableCell(const vector<string>& map, int x, int y, int turn, int N) {
    if (map[x][y] == '#') {
        // Simula una pared que se abre al turn N (ej. posición especial)
        if (x == 2 && y == 2 && turn >= N) return true;
        return false;
    }
    return true;
}

// Lee el map desde un file de texto
vector<string> loadmapFile(const string& filename) {
    ifstream file(filename);
    vector<string> map;
    string line;

    while (getline(file, line)) {
        if (!line.empty()) {
            map.push_back(line);
        }
    }

    return map;
}

// Aplica Dijkstra para encontrar el way desde 'S' hasta 'G'
vector<pair<int, int>> dijkstraAlgorithm(const vector<string>& map, int maxEnergy= 5, int unlockN = 6) {
    int n = map.size();
    int m = map[0].size();

    pair<int, int> start, finish;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j) {
            if (map[i][j] == 'S') start = {i, j};
            if (map[i][j] == 'G') finish = {i, j};
        }

    // 4D para evitar repetir States (x, y, energía, brokenWall)
    vector<vector<vector<vector<bool>>>> visited(n, vector(m, vector(maxEnergy+ 1, vector(2, false))));

    priority_queue<State, vector<State>, greater<>> pq;
    State inicial = {start.first, start.second, 0, 0, false, 0, {{start.first, start.second}}};
    pq.push(inicial);

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!pq.empty()) {
        State current = pq.top(); pq.pop();
        int x = current.x, y = current.y, energy = current.energy;
        bool brokenWall = current.brokenWall;

        if (x == finish.first && y == finish.second) {
            return current.way;
        }

        if (visited[x][y][energy][brokenWall]) continue;
        visited[x][y][energy][brokenWall] = true;

        for (int dir = 0; dir < 4; ++dir) {
            int nx = x + dx[dir], ny = y + dy[dir];
            if (nx < 0 || ny < 0 || nx >= n || ny >= m) continue;

            char celda = map[nx][ny];
            bool transitable = availableCell(map, nx, ny, current.turn + 1, unlockN);

            if (transitable || celda == '.' || celda == 'G' || celda == 'S') {
                State nextCell = current;
                nextCell.x = nx;
                nextCell.y = ny;
                nextCell.turn++;
                nextCell.energy = min(energy + 1, maxEnergy);
                nextCell.cost++;
                nextCell.way.push_back({nx, ny});
                pq.push(nextCell);
            } else if (celda == '#' && energy >= maxEnergy&& !brokenWall) {
                State nextCell = current;
                nextCell.x = nx;
                nextCell.y = ny;
                nextCell.turn++;
                nextCell.energy = 0;
                nextCell.brokenWall = true;
                nextCell.cost++;
                nextCell.way.push_back({nx, ny});
                pq.push(nextCell);
            }
        }
    }

    return {}; // No hay way posible
}

int main() {
    string mazeFile = "maze.txt";
    vector<string> map = loadmapFile(mazeFile);

    if (map.empty()) {
        cout << "Error: No se pudo cargar el file '" << mazeFile << "'\n";
        return 1;
    }

    auto way = dijkstraAlgorithm(map);

    if (way.empty()) {
        cout << "No se encontró un way desde S hasta G.\n";
    } else {
        cout << "way encontrado:\n";
        for (auto [x, y] : way)
            cout << "(" << x << ", " << y << ") ";
        cout << "\nTotal de pasos: " << way.size() - 1 << endl;
    }

    return 0;
}
