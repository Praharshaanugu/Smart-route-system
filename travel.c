#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 100

// -------- Structures --------
struct Node {
    int city, distance, cost, time;
    struct Node* next;
};

struct Graph {
    int numCities;
    struct Node* adjList[MAX];
};

struct Route {
    int path[MAX];
    int length;
    int totalCost;
};

// -------- Global --------
char cities[MAX][50];
struct Route routes[1000];
int routeCount = 0;

// -------- Graph Functions --------
struct Node* createNode(int city, int d, int c, int t) {
    struct Node* n = (struct Node*)malloc(sizeof(struct Node));
    n->city = city; n->distance = d; n->cost = c; n->time = t;
    n->next = NULL;
    return n;
}

struct Graph* createGraph(int n) {
    struct Graph* g = malloc(sizeof(struct Graph));
    g->numCities = n;
    for (int i = 0; i < n; i++) g->adjList[i] = NULL;
    return g;
}
void toLowerCase(char str[]) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] = str[i] + 32;
    }
}

void addEdge(struct Graph* g, int s, int d, int dist, int cost, int time) {
    struct Node* n = createNode(d, dist, cost, time);
    n->next = g->adjList[s];
    g->adjList[s] = n;

    n = createNode(s, dist, cost, time);
    n->next = g->adjList[d];
    g->adjList[d] = n;
}

// -------- Utility --------
int getCityIndex(char name[], int n) {
    char temp1[50], temp2[50];

    for (int i = 0; i < n; i++) {
        strcpy(temp1, cities[i]);
        strcpy(temp2, name);

        toLowerCase(temp1);
        toLowerCase(temp2);

        if (strcmp(temp1, temp2) == 0)
            return i;
    }
    return -1;
}

// -------- Dijkstra --------
int minDistance(int dist[], int vis[], int n) {
    int min = INT_MAX, idx = -1;
    for (int i = 0; i < n; i++)
        if (!vis[i] && dist[i] < min) min = dist[i], idx = i;
    return idx;
}

void printPath(int parent[], int j) {
    if (parent[j] == -1) return;
    printPath(parent, parent[j]);
    printf(" -> %s", cities[j]);
}

void dijkstra(struct Graph* g, int src, int dest, int choice, int avoidCity) {
    int n = g->numCities;
    int dist[MAX], vis[MAX], parent[MAX];

    for (int i = 0; i < n; i++)
        dist[i] = INT_MAX, vis[i] = 0, parent[i] = -1;

    dist[src] = 0;

    for (int i = 0; i < n - 1; i++) {
        int u = minDistance(dist, vis, n);
        if (u == -1) break;

        // Skip avoided city
        if (u == avoidCity) continue;

        vis[u] = 1;

        struct Node* temp = g->adjList[u];
        while (temp) {

            if (temp->city == avoidCity) {
                temp = temp->next;
                continue;
            }

            int w = (choice == 1) ? temp->cost :
                    (choice == 2) ? temp->distance : temp->time;

            if (!vis[temp->city] && dist[u] + w < dist[temp->city]) {
                dist[temp->city] = dist[u] + w;
                parent[temp->city] = u;
            }
            temp = temp->next;
        }
    }

    if (dist[dest] == INT_MAX) {
        printf("\n No valid route found!\n");
        return;
    }

    printf("\n BEST ROUTE:\n%s", cities[src]);
    printPath(parent, dest);
    printf("\nTotal Value: %d\n", dist[dest]);
}

// -------- DFS Store Routes --------
void storePaths(struct Graph* g, int src, int dest, int vis[],
                int path[], int idx, int cost, int avoidCity) {
    if (routeCount > 50) return;
    //  Skip if this city must be avoided
    if (src == avoidCity) return;

    vis[src] = 1;
    path[idx++] = src;

    if (src == dest) {
        routes[routeCount].length = idx;
        routes[routeCount].totalCost = cost;

        for (int i = 0; i < idx; i++)
            routes[routeCount].path[i] = path[i];

        routeCount++;
    } 
    else {
        struct Node* temp = g->adjList[src];

        while (temp) {
            if (!vis[temp->city]) {
                storePaths(g, temp->city, dest, vis, path,
                           idx, cost + temp->cost, avoidCity);
            }
            temp = temp->next;
        }
    }

    vis[src] = 0;
}

// -------- Sorting --------
void sortRoutes() {
    for (int i = 0; i < routeCount - 1; i++)
        for (int j = 0; j < routeCount - i - 1; j++)
            if (routes[j].totalCost > routes[j+1].totalCost) {
                struct Route t = routes[j];
                routes[j] = routes[j+1];
                routes[j+1] = t;
            }
}

// -------- Display --------
void displayTopWithBudget(int top, int budget) {
    printf("\n🔹 TOP ROUTES (Within Budget):\n");

    int count = 0;

    for (int i = 0; i < routeCount; i++) {

        // Apply budget filter
        if (budget == 0 || routes[i].totalCost <= budget) {

            printf("\nRoute %d: ", count + 1);

            for (int j = 0; j < routes[i].length; j++) {
                printf("%s", cities[routes[i].path[j]]);
                if (j != routes[i].length - 1)
                    printf(" -> ");
            }

            printf(" | Cost: %d", routes[i].totalCost);

            count++;
        }

        if (count == top) break;
    }

    if (count == 0) {
        printf("\n No routes found within budget!");
    }
}

// -------- Save to File --------
void saveToFile() {
    FILE* f = fopen("output.txt", "w");
    for (int i = 0; i < routeCount; i++) {
        for (int j = 0; j < routes[i].length; j++)
            fprintf(f, "%s ", cities[routes[i].path[j]]);
        fprintf(f, "| Cost: %d\n", routes[i].totalCost);
    }
    fclose(f);
}

int main(int argc, char *argv[]) {

    if (argc < 6) {
        printf("Error: Missing arguments\n");
        return 0;
    }

    char srcName[50], destName[50], avoidCity[50];
    int choice, budget;

    strcpy(srcName, argv[1]);
    strcpy(destName, argv[2]);
    choice = atoi(argv[3]);
    budget = atoi(argv[4]);
    strcpy(avoidCity, argv[5]);
    FILE *fc = fopen("cities.txt", "r");
    int n = 0;
    while (fscanf(fc, "%s", cities[n]) != EOF) n++;
    fclose(fc);

    struct Graph* g = createGraph(n);

    FILE *fr = fopen("routes.txt", "r");
    int s,d,dist,cost,time;
    while (fscanf(fr, "%d %d %d %d %d", &s,&d,&dist,&cost,&time) != EOF)
        addEdge(g, s, d, dist, cost, time);
    fclose(fr);

   int src = getCityIndex(srcName, n);
   int dest = getCityIndex(destName, n);
   int avoidIndex = (strcmp(avoidCity, "NONE") == 0) ? -1 : getCityIndex(avoidCity, n);

// Validation
  if (src == -1 || dest == -1) {
    printf("Invalid city!\n");
    return 0;
 }
 
  if (src == dest) {
    printf("Same source & destination!\n");
    return 0;
}

// Run Dijkstra
  dijkstra(g, src, dest, choice, avoidIndex);

// Run DFS
  int vis[MAX] = {0}, path[MAX];
  routeCount = 0;

  storePaths(g, src, dest, vis, path, 0, 0, avoidIndex);

  sortRoutes();
  displayTopWithBudget(3, budget);
  saveToFile();

    return 0;
}