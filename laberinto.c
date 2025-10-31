#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>   

#define MURO '#'      
#define CAMINO '.'    
#define ENTRADA 'E'   
#define SALIDA 'S'    
#define SOLUCION '*'  

int N = 10;
char matriz[100][100];
int visitado[100][100];
int padresX[100][100], padresY[100][100];

// movimientos: arriba, abajo, izquierda, derecha
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

// ---- Generar laberinto con DFS ----
void generarDFS(int x, int y) {
    matriz[x][y] = CAMINO;
    int dirs[4] = {0, 1, 2, 3};

    // mezclar direcciones al azar
    for (int i=0; i<4; i++) {
        int r = rand()%4;
        int tmp = dirs[i];
        dirs[i] = dirs[r];
        dirs[r] = tmp;
    }

    for (int i=0; i<4; i++) {
        int d = dirs[i];
        int nx = x + dx[d]*2, ny = y + dy[d]*2;
        if(nx>0 && ny>0 && nx<N-1 && ny<N-1 && matriz[nx][ny]==MURO) {
            matriz[x+dx[d]][y+dy[d]] = CAMINO;
            generarDFS(nx, ny);
        }
    }
}

// ---- Crear el laberinto completo ----
void generarLaberinto() {
    for (int i=0; i<N; i++)
        for(int j=0; j<N; j++)
            matriz[i][j] = MURO;

    generarDFS(1,1);

    matriz[1][1] = ENTRADA;

    // Buscar una salida en la fila inferior que sea camino
    for (int j = N-2; j >= 1; j--) {
        if (matriz[N-2][j] == CAMINO) {
            matriz[N-2][j] = SALIDA;
            break;
        }
    }
}

// ---- Mostrar la matriz en pantalla ----
void mostrar() {
    for (int i=0; i<N; i++) {
        for(int j=0; j<N; j++)
            printf("%c", matriz[i][j]);
        printf("\n");
    }
}

// ---- Buscar camino con BFS ----
bool bfs() {
    int qx[10000], qy[10000];
    int front = 0, rear = 0;

    for(int i=0; i<N; i++) 
        for(int j=0; j<N; j++) 
            visitado[i][j]=0;
    
    visitado[1][1] = 1;
    padresX[1][1] = padresY[1][1] = -1;
    qx[rear] = 1; qy[rear++] = 1;

    while(front < rear) {
        int x = qx[front], y = qy[front++];

        // ✅ si es salida, reconstruir el camino
        if(matriz[x][y] == SALIDA) {
            while (x!=-1 && y!=-1) {
                if (matriz[x][y]==CAMINO) matriz[x][y]=SOLUCION;
                int px = padresX[x][y], py = padresY[x][y];
                x=px; y=py;
            }
            return true;
        }

        for(int d=0; d<4; d++) {
            int nx = x+dx[d], ny = y+dy[d];
            if(nx>=0 && ny>=0 && nx<N && ny<N && !visitado[nx][ny] && matriz[nx][ny]!=MURO) {
                visitado[nx][ny] = 1;
                padresX[nx][ny] = x;
                padresY[nx][ny] = y;
                qx[rear] = nx;
                qy[rear++] = ny;
            }
        }
    }
    return false;
}

int main(int argc,char *argv[]) {
    if(argc>=2) N=atoi(argv[1]);
    if(N<5) N=10;

    srand(time(NULL));

    LARGE_INTEGER freq, t0, t1, t2, t3;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&t0);
    generarLaberinto();
    QueryPerformanceCounter(&t1);

    QueryPerformanceCounter(&t2);
    bool ok=bfs();
    QueryPerformanceCounter(&t3);

    printf("Laberinto generado:\n");
    mostrar();
    if(ok) {
        printf("\nLaberinto resuelto:\n");
        mostrar();
    } else printf("No tiene solucion\n");

    double gen_ms = (t1.QuadPart - t0.QuadPart) * 1000.0 / freq.QuadPart;
    double solve_ms = (t3.QuadPart - t2.QuadPart) * 1000.0 / freq.QuadPart;
    printf("\nTiempo Generacion: %.6f ms | Resolucion: %.6f ms\n", gen_ms, solve_ms);

    return 0;
}
