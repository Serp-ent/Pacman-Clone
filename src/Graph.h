#ifndef PACMAN_GRAPH_H
#define PACMAN_GRAPH_H

#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

class Board;

// Define a graph structure
class Graph {
  public:
    // Define a structure to represent a graph node
    struct BoxNode {
        // Position of the node
        int x, y;
        // Neighboring nodes (adjacent boxes)
        std::vector<BoxNode *> neighbors;
    };

    void setBoard(Board &b);

    // TODO: use unique ptr to prevent memory leak
    std::unordered_map<int, BoxNode *>
        nodes; // Map of node IDs to node pointers

    void addNode(int x, int y);
    // Connect two nodes (add an edge between them)
    void connectNodes(int x1, int y1, int x2, int y2);

    bool exists(int x1, int y1, int x2, int y2);

    int columns() const;
    int rows() const;

  private:
    Board *board = nullptr;
};

std::vector<Graph::BoxNode *>
breadthFirstSearch(Graph &graph, int startX, int startY, int endX, int endY);

#endif // !PACMAN_GRAPH_H
