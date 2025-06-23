#include <bits/stdc++.h>
using namespace std;

#define pii pair<int,int>
#define vpii vector<pii>

const int INF = 1e9;

class Candidate {
public:
    int node;
    int cost;
    int energy;
    bool brokenWall;
    vector<pii> path;

    Candidate(int node, int cost, int energy = 0, bool brokenWall = false, vector<pii> path = {})
        : node(node), cost(cost), energy(energy), brokenWall(brokenWall), path(path) {}
};

Candidate minVal(vector<Candidate>& candidates) {
    int minIndex = -1;
    Candidate minCand(-1, INF);

    for (int i = 0; i < candidates.size(); i++) {
        if (candidates[i].cost < minCand.cost) {
            minCand = candidates[i];
            minIndex = i;
        }
    }

    candidates.erase(candidates.begin() + minIndex);
    return minCand;
}

vector<string> cargarMapa(const string& filename) {
    ifstream file(filename);
    string linea;
    vector<string> mapa;
    while (getline(file, linea)) {
        if (!linea.empty()) mapa.push_back(linea);
    }
    return mapa;
}

int id(int x, int y, int cols) {
    return x * cols + y;
}

pii coord(int id, int cols) {
    return {id / cols, id % cols};
}

vector<pii> dijkstra(const vector<string>& mapa, int filas, int columnas, int startID, int goalID, int energiaMax = 5) {
    int totalNodos = filas * columnas;
    vector<bool> visited(totalNodos, false);
    vector<Candidate> candidates;

    pii startCoord = coord(startID, columnas);
    candidates.push_back(Candidate(startID, 0, 0, false, {startCoord}));

    while (!candidates.empty()) {
        Candidate u = minVal(candidates);
        if (visited[u.node]) continue;
        visited[u.node] = true;

        if (u.node == goalID) {
            return u.path;
        }

        pii [x, y] = coord(u.node, columnas);
        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};

        for (int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir], ny = y + dy[dir];
            if (nx < 0 || ny < 0 || nx >= filas || ny >= columnas) continue;

            char celda = mapa[nx][ny];
            int v = id(nx, ny, columnas);
            vector<pii> newPath = u.path;
            newPath.push_back({nx, ny});

            // Si es libre o meta
            if (celda == '.' || celda == 'G') {
                candidates.push_back(Candidate(v, u.cost + 1, min(u.energy + 1, energiaMax), u.brokenWall, newPath));
            }
            // Si es una pared, pero puede romperla
            else if (celda == '#' && u.energy >= energiaMax && !u.brokenWall) {
                candidates.push_back(Candidate(v, u.cost + 1, 0, true, newPath));
            }
        }
    }

    return {}; // No se encontró camino
}

int main() {
    vector<string> mapa = cargarMapa("maze.txt");
    int filas = mapa.size();
    int columnas = mapa[0].size();
    int totalNodos = filas * columnas;

    int startID = -1, goalID = -1;

    for (int x = 0; x < filas; x++) {
        for (int y = 0; y < columnas; y++) {
            if (mapa[x][y] == 'S') startID = id(x, y, columnas);
            if (mapa[x][y] == 'G') goalID = id(x, y, columnas);
        }
    }

    if (startID == -1 || goalID == -1) {
        cout << "No se encontró 'S' o 'G' en el mapa.\n";
        return 1;
    }

    vector<pii> camino = dijkstra(mapa, filas, columnas, startID, goalID);

    if (camino.empty()) {
        cout << "No se encontró un camino desde S hasta G.\n";
    } else {
        cout << "Camino encontrado:\n";
        for (auto [x, y] : camino)
            cout << "(" << x << ", " << y << ") ";
        cout << "\nTotal de pasos: " << camino.size() - 1 << endl;
    }

    return 0;
}