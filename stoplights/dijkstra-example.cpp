#include <iostream>
#include <queue>
#include <functional>

using namespace std;

#define ll long long int
#define INF (1000*1000*1000ll*1000)

struct node {
  int index;
  int dist;
  node(int a, int b) {
    index = a;
    dist = b;
  }
};

bool cmp(const node &a, const node &b) {
  return a.dist < b.dist;
}

const int N = 10;
ll graph_distances[N][N];

// Assume dist has already been initialized to INF
ll dijkstra(int start, vector<int> graph[], ll dist[], bool vis[], int end = -1) {
  priority_queue<node, cmp> pq = priority_queue<node, cmp>();
  node s = node(start, 0);
  pq.push(s);
  while (!pq.empty()) {
    node cur = pq.front();
    pq.pop();
    if (cur.dist != dist[cur.index]) continue;
    if (vis[cur.index]) continue;
    if (cur.index == end) {
      return cur.dist;
    }
    if (cur.dist == INF) {
      // Either means the target node was unreachable or that
      // our complete search for distances from source is done
      return -1;
    }
    vis[cur.index] = true;
    for (int neighbour : graph[cur.index]) {
      if (!vis[neighbour]) {
        ll new_dist = cur.dist+graph_distances[cur.index][neighbour];
        if (new_dist < dist[neighbour]) {
          dist[neighbour] = new_dist;
          pq.push(node(neighbour, new_dist));
        }
      }
    }
  }
  return -1;
}
