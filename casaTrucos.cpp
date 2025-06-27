#include <iostream> 
#include <fstream>    // Para leer el mapa del laberinto desde .txt)
#include <vector>     // Para usar vectores dinámicos (como matriz del laberinto)
#include <queue>      // Para usar una cola de prioridad (Dijkstra)
#include <tuple>      // Útil para pares/triples
#include <string>     // Para manipular strings las líneas o filas del laberinto

using namespace std;

#define pii pair<int,int>

const int INF = 1e9;
const int maxEnergy = 5;

struct State {
    int x, y;           // Coordenadas del jugador en el laberinto
    int cost;           // Costo acumulado (cantidad de pasos hasta hasta el momento)
    int energy;         // Energía acumulada (incrementa con cada paso)
    bool brokenWall;    // ¿la pared ha sido rota? true o false
    vector<pii> path;   // lista ordenada de celdas por las que pasó el algoritmo.

    bool operator>(const State& other) const {
        return cost > other.cost;
    }
};

vector<string> loadmapFile(const string& filename) {
    ifstream file(filename);
    string line;
    vector<string> map;
    while (getline(file, line)) {
        if (!line.empty()) {
            map.push_back(line);
        } 
    }
    return map;
}

// Aplica Dijkstra para encontrar el camino desde 'S' hasta 'G'
vector<pii> dijkstraAlgorithm(const vector<string>& map, pii start, pii finish) {
    int n = map.size();
    int m = map[0].size();

    // visited[x][y][energy][brokenWall]
    // 4D para evitar repetir States (x, y, energía, brokenWall)
    vector<vector<vector<vector<bool>>>> visited(n,
        vector<vector<vector<bool>>>(m,
        vector<vector<bool>>(maxEnergy + 1,
        vector<bool>(2, false))));

    priority_queue<State, vector<State>, greater<State>> pq;
    pq.push({start.first, start.second, 0, 0, false, {start}});

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!pq.empty()) {
        State u = pq.top(); pq.pop();

        // devuelve las celdas transitadas por el algoritmo (es decir, el camino desde 'S' hasta 'G')
        if (u.x == finish.first && u.y == finish.second)
        // LLegando a la meta (G), que está en las coordenadas finish.first, finish.second, retorna:
            return u.path;

        if (visited[u.x][u.y][u.energy][u.brokenWall]) continue;
        visited[u.x][u.y][u.energy][u.brokenWall] = true;

        for (int d = 0; d < 4; ++d) {
            int nx = u.x + dx[d];
            int ny = u.y + dy[d];

            if (nx < 0 || ny < 0 || nx >= n || ny >= m) continue;

            char cell = map[nx][ny];
            State nextCell = u;
            nextCell.x = nx;
            nextCell.y = ny;
            nextCell.path.push_back({nx, ny});
            nextCell.cost++;

            // Si la celda es transitable
            if (cell == '.' || cell == 'G') {
                nextCell.energy = min(u.energy + 1, maxEnergy);

                // Imprimir pasos que faltan para romper pared
                if (!u.brokenWall && nextCell.energy < maxEnergy) {
                    int restantes = maxEnergy - nextCell.energy;
                    cout << "En (" << nx << ", " << ny << "): faltan " 
                         << restantes << " paso(s) para poder romper una pared.\n";
                }

                pq.push(nextCell);
            }
            // Si es pared y puedes romperla
            else if (cell == '#' && u.energy >= maxEnergy && !u.brokenWall) {
                nextCell.energy = 0;
                nextCell.brokenWall = true;
                cout << "Pared rota en (" << nx << ", " << ny << ") usando energía máxima.\n";
                pq.push(nextCell);
            }
        }
    }

    return {}; // No hay camino
}

int main() {
    vector<string> map = loadmapFile("maze.txt");
    int filas = map.size();
    int columnas = map[0].size();

    pii start = {-1, -1}, finish = {-1, -1};

    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j) {
            if (map[i][j] == 'S') start = {i, j};
            if (map[i][j] == 'G') finish = {i, j};
        }

    if (start.first == -1 || finish.first == -1) {
        cout << "No se encontró 'S' o 'G' en el mapa.\n";
        return 1;
    }

    vector<pii> way = dijkstraAlgorithm(map, start, finish);

    if (way.empty()) {
        cout << "No se encontró un camino desde S hasta G.\n";
    } else {
        cout << "Camino encontrado:\n";
        for (auto [x, y] : way)
            cout << "(" << x << ", " << y << ") ";
        cout << "\nTotal de pasos: " << way.size() - 1 << endl;
    }

    return 0;
}