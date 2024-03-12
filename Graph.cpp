#include "Graph.h"
#include "Behaviors.h"
#include "Board.h"
#include <cstdio>

void Graph::setBoard(Board &b) {
    board = &b;

    // Add nodes to the graph for each box on the board
    for (int i = 0; i < board->rows(); ++i) {
        for (int j = 0; j < board->columns(); ++j) {
            addNode(i, j);
        }
    }

    // Connect neighboring nodes (boxes) in the graph
    for (int i = 0; i < board->rows(); ++i) {
        for (int j = 0; j < board->columns(); ++j) {
            // Connect to neighboring nodes if they are not walls
            if (board->getBox(i, j).getType() != Box::Type::wall) {

                // Connect to the right neighbor
                if (j < board->columns() - 1 &&
                    board->getBox(i, j + 1).getType() != Box::Type::wall) {
                    connectNodes(i, j, i, j + 1);
                }

                // Connect to the bottom neighbor
                if (i < board->rows() - 1 &&
                    board->getBox(i + 1, j).getType() != Box::Type::wall) {
                    connectNodes(i, j, i + 1, j);
                }
            }
        }
    }
}
int Graph::columns() const { return board->columns(); }
int Graph::rows() const { return board->rows(); }

// Add a node to the graph
void Graph::addNode(int x, int y) {
    int nodeId = x * board->columns() + y;
    nodes[nodeId] = new BoxNode{x, y};
}

// Connect two nodes (add an edge between them)
void Graph::connectNodes(int x1, int y1, int x2, int y2) {
    // TODO: don't connect if there already exists edge
    int nodeId1 = x1 * board->columns() + y1;
    int nodeId2 = x2 * board->columns() + y2;

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
    int startNodeId = startX * graph.columns() + startY;
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
                int nodeId = currentNode->x * graph.columns() + currentNode->y;
                currentNode = cameFrom[nodeId];
            }
            return path;
        }

        // Explore neighboring nodes
        for (auto &neighbor : currentNode->neighbors) {
            int neighborId = neighbor->x * graph.columns() + neighbor->y;
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
