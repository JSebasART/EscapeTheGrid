#include <bits/stdc++.h>
using namespace std;

#define pii pair<int,int>
#define vpii vector<pii>

const int INF = 1e9;

// Clase Candidate como en su ejemplo
class Candidate {
public:
    int node;
    int cost;

    Candidate(int node, int cost) {
        this->node = node;
        this->cost = cost;
    }
};

// Encuentra el candidato con menor costo
Candidate minVal(vector<Candidate> &candidates) {
    int minIndex = -1;
    Candidate minCand = Candidate(-1, INF);

    for (int i = 0; i < candidates.size(); i++) {
        if (candidates[i].cost < minCand.cost) {
            minCand = candidates[i];
            minIndex = i;
        }
    }

    candidates.erase(candidates.begin() + minIndex);
    return minCand;
}

// Dijkstra tradicional usando su lógica
vector<int> dijkstra(vector<vpii> &AL, int start) {
    vector<Candidate> candidates;
    candidates.push_back(Candidate(start, 0));
    vector<int> dists(AL.size(), INF);

    while (!candidates.empty()) {
        Candidate u = minVal(candidates);
        if (u.cost >= dists[u.node]) continue;
        dists[u.node] = u.cost;

        for (auto v : AL[u.node]) {
            int newCost = dists[u.node] + v.second;
            candidates.push_back(Candidate(v.first, newCost));
        }
    }

    return dists;
}

// Carga el mapa desde archivo
vector<string> cargarMapa(const string& filename) {
    ifstream file(filename);
    string linea;
    vector<string> mapa;
    while (getline(file, linea)) {
        if (!linea.empty()) mapa.push_back(linea);
    }
    return mapa;
}

// Convierte coordenadas (x,y) en ID de nodo
int id(int x, int y, int cols) {
    return x * cols + y;
}

// Convierte ID de nodo en coordenadas (x,y)
pii coord(int id, int cols) {
    return {id / cols, id % cols};
}

int main() {
    vector<string> mapa = cargarMapa("maze.txt");
    int filas = mapa.size();
    int columnas = mapa[0].size();
    int totalNodos = filas * columnas;

    vector<vpii> AL(totalNodos); // Lista de adyacencia

    int startID = -1, goalID = -1;

    // Construimos la lista de adyacencia
    for (int x = 0; x < filas; x++) {
        for (int y = 0; y < columnas; y++) {
            char c = mapa[x][y];
            if (c == '#') continue; // Pared

            int u = id(x, y, columnas);

            if (c == 'S') startID = u;
            if (c == 'G') goalID = u;

            // Movimientos arriba, abajo, izquierda, derecha
            int dx[] = {-1, 1, 0, 0};
            int dy[] = {0, 0, -1, 1};

            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                if (nx >= 0 && ny >= 0 && nx < filas && ny < columnas) {
                    char vecino = mapa[nx][ny];
                    if (vecino != '#') {
                        int v = id(nx, ny, columnas);
                        AL[u].push_back({v, 1}); // costo fijo 1
                    }
                }
            }
        }
    }

    if (startID == -1 || goalID == -1) {
        cout << "No se encontró 'S' o 'G' en el mapa.\n";
        return 1;
    }

    // Ejecutar Dijkstra
    vector<int> distancias = dijkstra(AL, startID);

    if (distancias[goalID] == INF) {
        cout << "No hay camino desde S hasta G.\n";
    } else {
        cout << "Costo mínimo desde S hasta G: " << distancias[goalID] << "\n";
    }

    return 0;
}