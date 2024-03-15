#include "Graph.h"
#include "Behaviors.h"
#include "Board.h"
#include <cstdio>

void Graph::setBoard(Board &b) {
    board = &b;

    // Add nodes to the graph for each box on the board
    for (int y = 0; y < board->rows(); ++y) {
        for (int x = 0; x < board->columns(); ++x) {
            addNode(x, y);
        }
    }

    // Connect neighboring nodes (boxes) in the graph
    for (int y = 0; y < board->rows(); ++y) {
        for (int x = 0; x < board->columns(); ++x) {
            // Connect to neighboring nodes if they are not walls
            if (board->getBox(x, y).getType() != Box::Type::wall) {
                // Connect to the right neighbor
                if (x < board->columns() - 1 &&
                    board->getBox(x + 1, y).getType() != Box::Type::wall) {
                    connectNodes(x, y, x + 1, y);
                }

                // Connect to the bottom neighbor
                if (y < board->rows() - 1 &&
                    board->getBox(x, y + 1).getType() != Box::Type::wall) {
                    connectNodes(x, y, x, y + 1);
                }
            }
        }
    }
}

int Graph::columns() const { return board->columns(); }
int Graph::rows() const { return board->rows(); }

// Add a node to the graph
void Graph::addNode(int x, int y) {
    int nodeId = y * board->columns() + x;
    nodes[nodeId] = new BoxNode{x, y};
}

// Connect two nodes (add an edge between them)
void Graph::connectNodes(int x1, int y1, int x2, int y2) {
    // TODO: don't connect if there already exists edge
    int nodeId1 = y1 * board->columns() + x1;
    int nodeId2 = y2 * board->columns() + x2;

    nodes[nodeId1]->neighbors.push_back(nodes[nodeId2]);
    nodes[nodeId2]->neighbors.push_back(nodes[nodeId1]);
}

// Breadth-First Search algorithm
std::vector<Graph::BoxNode *>
breadthFirstSearch(Graph &graph, int startX, int startY, int endX, int endY) {
    // store nodes to be explored
    std::queue<Graph::BoxNode *> frontier;
    // keep track of parent of each visited node during search
    std::unordered_map<int, Graph::BoxNode *> cameFrom;
    std::vector<bool> visited(graph.columns() * graph.rows(), false);

    // probably ghost current position
    int startNodeId = startY * graph.columns() + startX;
    Graph::BoxNode *startNode = graph.nodes[startNodeId];
    frontier.push(startNode);
    visited[startNodeId] = true;

    // main loop
    while (!frontier.empty()) { // continue to explore nodes
        Graph::BoxNode *currentNode = frontier.front();
        frontier.pop();

        // Check if we have reached the end node
        if (currentNode->x == endX && currentNode->y == endY) {
            // if we have reached end reconstruct path
            std::vector<Graph::BoxNode *> path;
            while (currentNode != nullptr) {
                path.push_back(currentNode);
                int nodeId = currentNode->y * graph.columns() + currentNode->x;
                currentNode = cameFrom[nodeId];
            }
            return path;
        }

        // Explore neighboring nodes
        for (auto &neighbor : currentNode->neighbors) {
            int neighborId = neighbor->y * graph.columns() + neighbor->x;
            if (visited[neighborId]) {
                continue;
            }

            // if (cameFrom.find(neighborId) == cameFrom.end()) {
            frontier.push(neighbor);
            cameFrom[neighborId] = currentNode;
            visited[neighborId] = true;
            // }
        }
    }

    return {}; // No path found
}
