#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <limits>

struct SPNode {
    sf::Vector2f position;
    int id;
    float distance;
    bool visited;
    SPNode(int id, sf::Vector2f pos) : id(id), position(pos), distance(std::numeric_limits<float>::max()), visited(false) {}
};

struct SPEdge {
    int u, v;
    float weight;
    bool inPath;
};

class ShortestPath {
private:
    std::vector<SPNode> nodes;
    std::vector<SPEdge> edges;
    sf::Font* font;
    std::string statusMsg = "Shortest Path: L-Click node, R-Click connect. Press S(Start), C(Clear)";
    int selectedNode = -1;
    
    int currentNode = -1;
    int goalNode = -1;
    bool isResultMode = false;
    bool isSelectingGoal = false;
    sf::Clock animClock;
    std::vector<int> parent;

public:
    ShortestPath(sf::Font* f) : font(f) {}

    void addNode(sf::Vector2f pos) {
        if (isSelectingGoal) {
            // Select goal node instead of adding
            for (int i = 0; i < nodes.size(); ++i) {
                float dx = nodes[i].position.x - pos.x;
                float dy = nodes[i].position.y - pos.y;
                if (dx*dx + dy*dy < 400) {
                    goalNode = i;
                    isSelectingGoal = false;
                    isResultMode = true;
                    statusMsg = "Shortest Path to Node " + std::to_string(goalNode) + " found!";
                    break;
                }
            }
            return;
        }
        isResultMode = false;
        goalNode = -1;
        int newId = nodes.size();
        nodes.push_back(SPNode(newId, pos));
        statusMsg = "Added node " + std::to_string(newId) + ". R-Click nodes to connect.";
    }

    void clear() {
        nodes.clear();
        edges.clear();
        currentNode = -1;
        goalNode = -1;
        isSelectingGoal = false;
        isResultMode = false;
        statusMsg = "Shortest Path: L-Click node, R-Click connect. Press S(Start), C(Clear)";
    }

    void handleRightClick(sf::Vector2f pos) {
        isResultMode = false;
        int clicked = -1;
        for (int i = 0; i < nodes.size(); ++i) {
            float dx = nodes[i].position.x - pos.x;
            float dy = nodes[i].position.y - pos.y;
            if (dx*dx + dy*dy < 400) {
                clicked = i;
                break;
            }
        }
        if (clicked != -1) {
            if (selectedNode == -1) {
                selectedNode = clicked;
            } else {
                float dx = nodes[selectedNode].position.x - nodes[clicked].position.x;
                float dy = nodes[selectedNode].position.y - nodes[clicked].position.y;
                float dist = std::sqrt(dx*dx + dy*dy);
                edges.push_back({selectedNode, clicked, dist, false});
                edges.push_back({clicked, selectedNode, dist, false}); // directed representation for undirected graph
                selectedNode = -1;
            }
        }
    }

    void startDijkstra() {
        if (nodes.empty()) return;
        for (auto& n : nodes) { n.distance = std::numeric_limits<float>::max(); n.visited = false; }
        for (auto& e : edges) e.inPath = false;
        
        parent.assign(nodes.size(), -1);
        nodes[0].distance = 0;
        
        // Run full Dijkstra instantly to get parents
        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
        pq.push({0, 0});
        
        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();
            if (nodes[u].visited) continue;
            nodes[u].visited = true;
            
            for (auto& edge : edges) {
                if (edge.u == u && !nodes[edge.v].visited) {
                    if (nodes[u].distance + edge.weight < nodes[edge.v].distance) {
                        nodes[edge.v].distance = nodes[u].distance + edge.weight;
                        parent[edge.v] = u;
                        pq.push({nodes[edge.v].distance, edge.v});
                    }
                }
            }
        }
        
        isSelectingGoal = true;
        statusMsg = "Dijkstra: Done. Now LEFT-CLICK any node to set it as your Goal.";
    }

    void update() {
        if (currentNode != -1 && animClock.getElapsedTime().asSeconds() > 1.0f) {
            nodes[currentNode].visited = true;

            // Relax edges
            for (auto& edge : edges) {
                if (edge.u == currentNode && !nodes[edge.v].visited) {
                    if (nodes[currentNode].distance + edge.weight < nodes[edge.v].distance) {
                        nodes[edge.v].distance = nodes[currentNode].distance + edge.weight;
                        parent[edge.v] = currentNode;
                    }
                }
            }

            // Find next min
            float minD = std::numeric_limits<float>::max();
            int nextNode = -1;
            for (int i = 0; i < nodes.size(); i++) {
                if (!nodes[i].visited && nodes[i].distance < minD) {
                    minD = nodes[i].distance;
                    nextNode = i;
                }
            }
            
            currentNode = nextNode;
            if (currentNode == -1) statusMsg = "Dijkstra Finished!";
            animClock.restart();
        }
    }

    void draw(sf::RenderWindow& window) {
        if (font) {
            sf::Text text(*font, statusMsg, 20);
            text.setFillColor(sf::Color(241, 196, 15));
            text.setPosition({10, 50});
            window.draw(text);
        }

        // Draw Edges
        std::vector<bool> pathEdges(edges.size(), false);
        if (isResultMode && goalNode != -1) {
            int curr = goalNode;
            while (curr != -1 && parent[curr] != -1) {
                int p = parent[curr];
                for (int i = 0; i < edges.size(); ++i) {
                    if ((edges[i].u == p && edges[i].v == curr) || (edges[i].u == curr && edges[i].v == p)) {
                        pathEdges[i] = true;
                    }
                }
                curr = p;
            }
        }

        for (auto& edge : edges) {
            if (edge.u > edge.v) continue;
            int idx = -1;
            // Find directed index to check pathEdges
            for(int i=0; i<edges.size(); ++i) {
                if((edges[i].u == edge.u && edges[i].v == edge.v) || (edges[i].u == edge.v && edges[i].v == edge.u)) {
                    if(pathEdges[i]) { idx = i; break; }
                }
            }
            
            bool isInPath = (idx != -1);
            bool shouldDraw = !isResultMode || isInPath;
            sf::Color color = isInPath ? sf::Color::White : sf::Color(200, 200, 200, 100);
            
            if (shouldDraw) {
                sf::Vertex line[] = { sf::Vertex{nodes[edge.u].position, color}, 
                                      sf::Vertex{nodes[edge.v].position, color} };
                window.draw(line, 2, sf::PrimitiveType::Lines);

                if (font) {
                    sf::Vector2f mid = (nodes[edge.u].position + nodes[edge.v].position) / 2.0f;
                    sf::Text text(*font, std::to_string((int)edge.weight), 14);
                    text.setFillColor(color);
                    text.setPosition(mid);
                    window.draw(text);
                }
            }
        }

        for (auto& node : nodes) {
            sf::CircleShape circle(20);
            circle.setOrigin({20, 20});
            circle.setPosition(node.position);
            
            sf::Color fillCol = sf::Color(52, 152, 219);
            sf::Color outCol = sf::Color(41, 128, 185);
            if (node.id == currentNode) fillCol = sf::Color(231, 76, 60);
            else if (node.id == selectedNode) fillCol = sf::Color(155, 89, 182);
            
            circle.setFillColor(fillCol);
            circle.setOutlineThickness(2.f);
            circle.setOutlineColor(outCol);
            window.draw(circle);

            if (font) {
                std::string dStr = (node.distance > 1e6) ? "INF" : std::to_string((int)node.distance);
                sf::Text text(*font, std::to_string(node.id) + "\n" + dStr, 14);
                text.setFillColor(sf::Color::White);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setOrigin({bounds.size.x / 2.0f + bounds.position.x, bounds.size.y / 2.0f + bounds.position.y});
                text.setPosition(node.position);
                window.draw(text);
            }
        }
    }
};
