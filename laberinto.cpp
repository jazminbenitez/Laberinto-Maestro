// laberinto.cpp
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using pii = pair<int,int>;

int H = 10, W = 10; // tamaño por defecto (celdas)

std::mt19937 rng((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());

// tamaño del "tablero" para mostrar: filas = 2*H+1, cols = 2*W+1
vector<string> maze_display;

// auxiliares: ¿está (r,c) dentro de las celdas?
bool in_cells(int r,int c){ return r>=0 && r<H && c>=0 && c<W; }

// Crear laberinto usando DFS aleatorio (maze "perfecto": una sola ruta entre celdas)
void generate_maze_dfs(){
    int R = 2*H+1, C = 2*W+1;
    maze_display.assign(R, string(C, '#')); // lleno de muros '#'
    // marcar las posiciones de las celdas como espacios (centros)
    for(int i=0;i<H;i++) for(int j=0;j<W;j++)
        maze_display[2*i+1][2*j+1] = ' ';

    vector<vector<bool>> vis(H, vector<bool>(W,false));
    vector<pair<int,int>> stack;
    stack.reserve(H*W);
    stack.push_back({0,0});
    vis[0][0] = true;

    while(!stack.empty()){
        auto [r,c] = stack.back();
        // recopilar vecinos no visitados
        vector<pii> neigh;
        int dr[4] = {-1,1,0,0}, dc[4] = {0,0,-1,1};
        for(int k=0;k<4;k++){
            int nr=r+dr[k], nc=c+dc[k];
            if(in_cells(nr,nc) && !vis[nr][nc]) neigh.push_back({nr,nc});
        }

        if(neigh.empty()){
            stack.pop_back(); // retrocede si no hay vecinos nuevos
        } else {
            // elegir vecino aleatorio
            uniform_int_distribution<int> dist(0, (int)neigh.size()-1);
            auto [nr,nc] = neigh[dist(rng)];
            // quitar la pared entre (r,c) y (nr,nc)
            // calculamos la posición real de la pared en maze_display (entre dos centros)
            int ar = 2*r+1, ac = 2*c+1;
            int br = 2*nr+1, bc = 2*nc+1;
            int wallr = (ar+br)/2, wallc = (ac+bc)/2;
            maze_display[wallr][wallc] = ' ';
            vis[nr][nc] = true;
            stack.push_back({nr,nc});
        }
    }
    // abrir entrada y salida externas (bordes)
    maze_display[1][0] = ' '; // entrada (izquierda de la celda inicial)
    maze_display[2*H-1][2*W] = ' '; // salida (derecha de la celda final)
}

// BFS para encontrar el camino más corto y luego marcarlo con '*'
bool solve_bfs_and_mark(){
    vector<vector<int>> parent(H, vector<int>(W,-1)); // guarda padre como r*W+c
    queue<pii> q;
    q.push({0,0});
    parent[0][0] = -2; // marca la raíz (inicio)
    int dr[4] = {-1,1,0,0}, dc[4] = {0,0,-1,1};

    while(!q.empty()){
        auto [r,c] = q.front(); q.pop();
        if(r==H-1 && c==W-1) break; // si llegamos a la salida, paramos

        for(int k=0;k<4;k++){
            int nr=r+dr[k], nc=c+dc[k];
            if(!in_cells(nr,nc)) continue;       // fuera del tablero de celdas
            if(parent[nr][nc] != -1) continue;   // ya visitado

            // comprobamos si hay paso entre (r,c) y (nr,nc) mirando la celda pared intermedia
            int ar=2*r+1, ac=2*c+1, br=2*nr+1, bc=2*nc+1;
            int wallr=(ar+br)/2, wallc=(ac+bc)/2;
            if(maze_display[wallr][wallc] == ' '){
                parent[nr][nc] = r*W + c; // guardamos padre
                q.push({nr,nc});
            }
        }
    }

    if(parent[H-1][W-1] == -1) return false; // no hay solución (raro)

    // reconstruir camino desde la salida hacia el inicio y marcar con '*'
    int cur = (H-1)*W + (W-1);
    while(cur != -2){
        int r = cur / W, c = cur % W;
        // marcar centro de la celda con '*'
        maze_display[2*r+1][2*c+1] = '*';
        int p = parent[r][c];
        if(p == -2) break; // llegamos al inicio
        if(p >= 0){
            int pr = p / W, pc = p % W;
            int wallr = (2*r+1 + 2*pr+1)/2;
            int wallc = (2*c+1 + 2*pc+1)/2;
            // marcar la pared entre celdas como parte del camino
            maze_display[wallr][wallc] = '*';
        }
        cur = p;
    }

    // marcar inicio y fin con S y E (también en los bordes)
    maze_display[1][0] = 'S';
    maze_display[2*H-1][2*W] = 'E';
    maze_display[2*0+1][2*0+1] = 'S';
    maze_display[2*(H-1)+1][2*(W-1)+1] = 'E';
    return true;
}

int main(int argc, char** argv){
    // leer argumentos: ./laberinto H W  ó  ./laberinto n  (n x n)
    if(argc>=3){
        H = max(2, atoi(argv[1]));
        W = max(2, atoi(argv[2]));
    } else {
        if(argc==2){ int n = max(2, atoi(argv[1])); H=W=n; }
    }

    // medir tiempo de generación
    auto t0 = chrono::high_resolution_clock::now();
    generate_maze_dfs();
    auto t1 = chrono::high_resolution_clock::now();
    double gen_ms = chrono::duration<double, milli>(t1-t0).count();

    // medir tiempo de resolución
    auto t2 = chrono::high_resolution_clock::now();
    bool solved = solve_bfs_and_mark();
    auto t3 = chrono::high_resolution_clock::now();
    double solve_ms = chrono::duration<double, milli>(t3-t2).count();

    // imprimir el laberinto en consola
    for(auto &row : maze_display) cout << row << "\n";
    cout << "\nGeneración: " << gen_ms << " ms | Resolución: " << solve_ms << " ms\n";
    if(!solved) cout << "No se encontró solución (algo raro pasó).\n";
    return 0;
}
