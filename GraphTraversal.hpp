#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <string>
#include <iostream>

struct GraphNode {
    sf::Vector2f position;
    int id;
    sf::Color color; // White=Unvisited, Gray=Visiting, Black=Visited
    int left = -1;
    int right = -1;
    GraphNode(int id, sf::Vector2f pos) : id(id), position(pos), color(sf::Color::White) {}
};

class GraphTraversal {
private:
    std::vector<GraphNode> nodes;
    std::vector<std::pair<int, int>> edges;
    sf::Font* font;
    std::string statusMsg = "Graph: Left click to add node (auto BST). Press B(BFS), D(DFS)";
    int selectedNode = -1;

    // Animation queue
    std::vector<std::vector<std::pair<int, sf::Color>>> animFrames;
    int animIndex = 0;
    sf::Clock animClock;

public:
    GraphTraversal(sf::Font* f) : font(f) {}

    void addNode(sf::Vector2f pos) {
        int newId = nodes.size();
        nodes.push_back(GraphNode(newId, pos));
        if (newId > 0) {
            int curr = 0;
            while (true) {
                if (pos.x < nodes[curr].position.x) {
                    if (nodes[curr].left == -1) {
                        nodes[curr].left = newId;
                        edges.push_back({curr, newId});
                        edges.push_back({newId, curr}); // undirected
                        break;
                    } else {
                        curr = nodes[curr].left;
                    }
                } else {
                    if (nodes[curr].right == -1) {
                        nodes[curr].right = newId;
                        edges.push_back({curr, newId});
                        edges.push_back({newId, curr}); // undirected
                        break;
                    } else {
                        curr = nodes[curr].right;
                    }
                }
            }
        }
    }

    void handleRightClick(sf::Vector2f pos) {
        int clicked = -1;
        for (int i = 0; i < nodes.size(); ++i) {
            float dx = nodes[i].position.x - pos.x;
            float dy = nodes[i].position.y - pos.y;
            if (dx*dx + dy*dy < 400) { // radius 20 squared
                clicked = i;
                break;
            }
        }
        if (clicked != -1) {
            if (selectedNode == -1) {
                selectedNode = clicked;
                statusMsg = "Selected node " + std::to_string(clicked) + " to connect.";
            } else {
                edges.push_back({selectedNode, clicked});
                edges.push_back({clicked, selectedNode}); // undirected
                selectedNode = -1;
                statusMsg = "Edge added.";
            }
        }
    }

    void startBFS() {
        if (nodes.empty()) return;
        resetColors();
        animFrames.clear();
        std::vector<bool> visited(nodes.size(), false);
        std::queue<int> q;

        int start = 0;
        if (selectedNode != -1) start = selectedNode;

        q.push(start);
        visited[start] = true;
        animFrames.push_back({{start, sf::Color(241, 196, 15)}}); // Yellow for Discovered

        while (!q.empty()) {
            int levelSize = q.size();
            std::vector<std::pair<int, sf::Color>> visitingSteps;
            std::vector<std::pair<int, sf::Color>> discoveringSteps;
            
            for (int i = 0; i < levelSize; ++i) {
                int u = q.front();
                q.pop();
                
                visitingSteps.push_back({u, sf::Color(46, 204, 113)}); // Green for Visited

                for (auto edge : edges) {
                    if (edge.first == u && !visited[edge.second]) {
                        visited[edge.second] = true;
                        q.push(edge.second);
                        discoveringSteps.push_back({edge.second, sf::Color(241, 196, 15)});
                    }
                }
            }
            if (!visitingSteps.empty()) animFrames.push_back(visitingSteps);
            if (!discoveringSteps.empty()) animFrames.push_back(discoveringSteps);
        }
        animIndex = 0;
        statusMsg = "Running BFS (Level by Level)...";
    }

    void dfsHelper(int u, std::vector<bool>& visited) {
        visited[u] = true;
        animFrames.push_back({{u, sf::Color(46, 204, 113)}}); // Green for Visiting

        for (auto edge : edges) {
            if (edge.first == u && !visited[edge.second]) {
                dfsHelper(edge.second, visited);
            }
        }
        
        animFrames.push_back({{u, sf::Color(231, 76, 60)}}); // Red for Backtracked
    }

    void startDFS() {
        if (nodes.empty()) return;
        resetColors();
        animFrames.clear();
        std::vector<bool> visited(nodes.size(), false);
        
        int start = 0;
        if (selectedNode != -1) start = selectedNode;
        
        dfsHelper(start, visited);
        animIndex = 0;
        statusMsg = "Running DFS (Depth then Backtrack)...";
    }

    void resetColors() {
        for (auto& n : nodes) n.color = sf::Color::White;
    }

    void update() {
        if (animIndex < animFrames.size() && animClock.getElapsedTime().asSeconds() > 0.5f) {
            for (auto& step : animFrames[animIndex]) {
                nodes[step.first].color = step.second;
            }
            animIndex++;
            animClock.restart();
        }
    }

    void draw(sf::RenderWindow& window) {
        if (font) {
            sf::Text text(*font, statusMsg, 20);
            text.setFillColor(sf::Color(241, 196, 15)); // Yellowish
            text.setPosition({10, 50});
            window.draw(text);
        }

        // Draw edges
        for (auto edge : edges) {
            sf::Vertex line[] = { sf::Vertex{nodes[edge.first].position, sf::Color(189, 195, 199)}, 
                                  sf::Vertex{nodes[edge.second].position, sf::Color(189, 195, 199)} };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        // Draw nodes
        for (auto& node : nodes) {
            // Shadow
            sf::CircleShape shadow(20);
            shadow.setOrigin({20, 20});
            shadow.setPosition(node.position + sf::Vector2f(3.f, 3.f));
            shadow.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(shadow);

            sf::CircleShape circle(20);
            circle.setOrigin({20, 20});
            circle.setPosition(node.position);
            
            sf::Color fillCol = sf::Color(52, 152, 219); // Default Light Blue
            sf::Color outCol = sf::Color(41, 128, 185); // Darker Blue
            
            if (node.color != sf::Color::White) {
                fillCol = node.color; // Used for visited
                outCol = sf::Color(39, 174, 96);
            }
            if (node.id == selectedNode && node.color == sf::Color::White) {
                fillCol = sf::Color(155, 89, 182); // Purple
                outCol = sf::Color(142, 68, 173);
            }
            
            circle.setFillColor(fillCol);
            circle.setOutlineThickness(3.f);
            circle.setOutlineColor(outCol);
            window.draw(circle);

            if (font) {
                sf::Text text(*font, std::to_string(node.id), 16);
                text.setFillColor(sf::Color::White);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setOrigin({bounds.size.x / 2.0f + bounds.position.x, bounds.size.y / 2.0f + bounds.position.y});
                text.setPosition(node.position);
                window.draw(text);
            }
        }
    }
};
