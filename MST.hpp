#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

struct MSTNode {
    sf::Vector2f position;
    int id;
    MSTNode(int id, sf::Vector2f pos) : id(id), position(pos) {}
};

struct MSTEdge {
    int u, v;
    float weight;
    bool isMST;
    bool isConsidered;
};

class MST {
private:
    std::vector<MSTNode> nodes;
    std::vector<MSTEdge> edges;
    sf::Font* font;
    std::string statusMsg = "MST: L-Click node, R-Click connect. Press K(Kruskal), P(Prim), C(Clear)";
    int selectedNode = -1;

    // Animation state
    int animStep = 0;
    std::vector<MSTEdge> sortedEdges;
    bool isResultMode = false;
    std::vector<int> parent;
    float totalCost = 0;
    sf::Clock animClock;

    int findParent(int i) {
        if (parent[i] == i) return i;
        return parent[i] = findParent(parent[i]);
    }

    void unionNodes(int i, int j) {
        int rootI = findParent(i);
        int rootJ = findParent(j);
        if (rootI != rootJ) parent[rootI] = rootJ;
    }

    bool isRunning = false;
    std::string algoType = "";
    std::vector<bool> visited; // For Prim's
    std::vector<MSTEdge> mstEdges;

public:
    MST(sf::Font* f) : font(f) {}

    void addNode(sf::Vector2f pos) {
        isResultMode = false;
        int newId = nodes.size();
        nodes.push_back(MSTNode(newId, pos));
        statusMsg = "Added node " + std::to_string(newId) + ". R-Click nodes to connect.";
    }

    void clear() {
        nodes.clear();
        edges.clear();
        mstEdges.clear();
        isRunning = false;
        isResultMode = false;
        totalCost = 0;
        statusMsg = "MST: L-Click node, R-Click connect. Press K(Kruskal), P(Prim)";
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
                edges.push_back({selectedNode, clicked, dist, false, false});
                selectedNode = -1;
            }
        }
    }

    void startKruskal() {
        if (nodes.empty()) return;
        isRunning = true;
        isResultMode = true;
        algoType = "Kruskal";
        for (auto& e : edges) { e.isMST = false; e.isConsidered = false; }
        mstEdges.clear();
        sortedEdges = edges;
        std::sort(sortedEdges.begin(), sortedEdges.end(), [](const MSTEdge& a, const MSTEdge& b) {
            return a.weight < b.weight;
        });
        parent.assign(nodes.size(), 0);
        for (int i = 0; i < nodes.size(); i++) parent[i] = i;
        animStep = 0;
        totalCost = 0;
        statusMsg = "Kruskal's: Adding edges from smallest to largest...";
        animClock.restart();
    }

    void startPrim() {
        if (nodes.empty()) return;
        isRunning = true;
        isResultMode = true;
        algoType = "Prim";
        for (auto& e : edges) { e.isMST = false; e.isConsidered = false; }
        mstEdges.clear();
        visited.assign(nodes.size(), false);
        visited[0] = true;
        animStep = 0;
        totalCost = 0;
        statusMsg = "Prim's: Starting from Node 0...";
        animClock.restart();
    }

    void update() {
        if (!isRunning) return;

        if (animClock.getElapsedTime().asSeconds() > 0.8f) {
            if (algoType == "Kruskal") {
                if (animStep < sortedEdges.size()) {
                    MSTEdge& se = sortedEdges[animStep];
                    se.isConsidered = true;
                    
                    // Find corresponding edge in original 'edges' to mark it
                    for (auto& e : edges) {
                        if ((e.u == se.u && e.v == se.v) || (e.u == se.v && e.v == se.u)) {
                            e.isConsidered = true;
                            if (findParent(se.u) != findParent(se.v)) {
                                e.isMST = true;
                                unionNodes(se.u, se.v);
                                totalCost += e.weight;
                                mstEdges.push_back(e);
                                statusMsg = "Kruskal: Added edge " + std::to_string(e.u) + "-" + std::to_string(e.v) + " (Cost: " + std::to_string((int)totalCost) + ")";
                            } else {
                                statusMsg = "Kruskal: Skipping cycle...";
                            }
                            break;
                        }
                    }
                    animStep++;
                } else {
                    isRunning = false;
                    statusMsg = "Kruskal Completed. Final MST Cost: " + std::to_string((int)totalCost);
                }
            } else if (algoType == "Prim") {
                float minWeight = 1e9;
                int minIdx = -1;
                for (int i = 0; i < edges.size(); ++i) {
                    bool uVis = visited[edges[i].u];
                    bool vVis = visited[edges[i].v];
                    if ((uVis && !vVis) || (!uVis && vVis)) {
                        if (edges[i].weight < minWeight) {
                            minWeight = edges[i].weight;
                            minIdx = i;
                        }
                    }
                }

                if (minIdx != -1) {
                    edges[minIdx].isMST = true;
                    visited[edges[minIdx].u] = true;
                    visited[edges[minIdx].v] = true;
                    totalCost += edges[minIdx].weight;
                    mstEdges.push_back(edges[minIdx]);
                    statusMsg = "Prim: Added edge " + std::to_string(edges[minIdx].u) + "-" + std::to_string(edges[minIdx].v);
                    animStep++;
                } else {
                    isRunning = false;
                    statusMsg = "Prim Completed. Final MST Cost: " + std::to_string((int)totalCost);
                }
            }
            animClock.restart();
        }
    }

    void draw(sf::RenderWindow& window) {
        if (font) {
            // Background box for status
            sf::RectangleShape statusBg({1180.f, 40.f});
            statusBg.setPosition({10.f, 50.f});
            statusBg.setFillColor(sf::Color(44, 49, 58, 200));
            statusBg.setOutlineColor(sf::Color(97, 175, 239));
            statusBg.setOutlineThickness(1.f);
            window.draw(statusBg);

            sf::Text text(*font, statusMsg, 18);
            text.setFillColor(sf::Color(229, 192, 123));
            text.setPosition({20, 58});
            window.draw(text);

            if (!isRunning && !mstEdges.empty()) {
                sf::Text costText(*font, "TOTAL MST COST: " + std::to_string((int)totalCost), 24);
                costText.setFillColor(sf::Color(46, 204, 113));
                costText.setPosition({450, 100});
                window.draw(costText);
            }
        }

        // Draw Edges
        for (auto& edge : edges) {
            bool shouldDraw = false;
            sf::Color color = sf::Color(200, 200, 200); // Standard light gray
            float thickness = 1.0f;

            if (!isResultMode) {
                // Building mode: show all edges simply
                shouldDraw = true;
            } else {
                // Result mode: only show MST edges
                if (edge.isMST) {
                    shouldDraw = true;
                    color = sf::Color::White; // Clean white for MST
                    thickness = 2.0f;
                }
            }

            if (shouldDraw) {
                if (thickness > 1.0f) {
                    sf::Vertex line[] = { 
                        sf::Vertex{nodes[edge.u].position + sf::Vector2f(0.5f, 0.5f), color}, 
                        sf::Vertex{nodes[edge.v].position + sf::Vector2f(0.5f, 0.5f), color},
                        sf::Vertex{nodes[edge.u].position - sf::Vector2f(0.5f, 0.5f), color}, 
                        sf::Vertex{nodes[edge.v].position - sf::Vector2f(0.5f, 0.5f), color}
                    };
                    window.draw(line, 4, sf::PrimitiveType::Lines);
                } else {
                    sf::Vertex line[] = { 
                        sf::Vertex{nodes[edge.u].position, color}, 
                        sf::Vertex{nodes[edge.v].position, color} 
                    };
                    window.draw(line, 2, sf::PrimitiveType::Lines);
                }

                // Draw weight
                if (font) {
                    sf::Vector2f mid = (nodes[edge.u].position + nodes[edge.v].position) / 2.0f;
                    sf::Text wText(*font, std::to_string((int)edge.weight), 15);
                    wText.setFillColor(color);
                    wText.setPosition(mid);
                    window.draw(wText);
                }
            }
        }

        for (auto& node : nodes) {
            sf::CircleShape circle(20);
            circle.setOrigin({20, 20});
            circle.setPosition(node.position);
            
            sf::Color fillCol = sf::Color(52, 152, 219);
            sf::Color outCol = sf::Color(41, 128, 185);
            if (node.id == selectedNode) {
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
