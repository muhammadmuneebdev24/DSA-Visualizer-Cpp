#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

class Heap {
private:
    std::vector<int> data;
    sf::Font* font;
    std::string statusMsg = "Press 'I' to add node, 'H' to Heapify-Up, 'D' to Delete/Heapify-Down";
    bool isMinHeap = true;
    
    // Step-by-step animation state
    enum class State { IDLE, HEAPIFY_UP, HEAPIFY_DOWN };
    State state = State::IDLE;
    int currentIdx = -1;
    sf::Clock stepClock;
    float stepInterval = 0.8f;

    // We'll store target positions for smooth animation
    struct NodeAnim {
        sf::Vector2f currentPos;
        sf::Vector2f targetPos;
    };
    std::vector<NodeAnim> animations;

    void processHeapifyUp() {
        if (currentIdx <= 0) {
            state = State::IDLE;
            statusMsg = "Insertion complete.";
            return;
        }

        int parent = (currentIdx - 1) / 2;
        bool shouldSwap = isMinHeap ? (data[currentIdx] < data[parent]) : (data[currentIdx] > data[parent]);

        if (shouldSwap) {
            statusMsg = "Swapping " + std::to_string(data[currentIdx]) + " and " + std::to_string(data[parent]) + 
                        (isMinHeap ? " because child is smaller than parent." : " because child is larger than parent.");
            std::swap(data[currentIdx], data[parent]);
            currentIdx = parent;
        } else {
            state = State::IDLE;
            statusMsg = "Heap property satisfied. No more changes needed.";
        }
    }

    void processHeapifyDown() {
        int left = 2 * currentIdx + 1;
        int right = 2 * currentIdx + 2;
        int target = currentIdx;

        if (left < data.size()) {
            bool condition = isMinHeap ? (data[left] < data[target]) : (data[left] > data[target]);
            if (condition) target = left;
        }
        if (right < data.size()) {
            bool condition = isMinHeap ? (data[right] < data[target]) : (data[right] > data[target]);
            if (condition) target = right;
        }

        if (target != currentIdx) {
            statusMsg = "Swapping " + std::to_string(data[currentIdx]) + " with " + std::to_string(data[target]) + 
                        " to move the " + (isMinHeap ? "smaller" : "larger") + " value up.";
            std::swap(data[currentIdx], data[target]);
            currentIdx = target;
        } else {
            state = State::IDLE;
            statusMsg = "Heapify-down complete. Root is now the " + std::string(isMinHeap ? "minimum" : "maximum") + ".";
        }
    }

public:
    Heap(sf::Font* f) : font(f) {}

    void insert(int key) {
        data.push_back(key);
        NodeAnim anim;
        anim.currentPos = sf::Vector2f(600, 700);
        animations.push_back(anim);
        statusMsg = "Added " + std::to_string(key) + " at end. Press 'H' to Heapify.";
    }

    void startHeapifyUp() {
        if (data.empty() || state != State::IDLE) return;
        currentIdx = data.size() - 1;
        state = State::HEAPIFY_UP;
        stepClock.restart();
    }

    void deleteRoot() {
        if (data.empty() || state != State::IDLE) return;
        statusMsg = "Moved last to root. Press 'H' to Heapify-Down.";
        data[0] = data.back();
        data.pop_back();
        animations.pop_back();
        currentIdx = 0;
    }
    
    void startHeapifyDown() {
        if (data.empty() || state != State::IDLE) return;
        state = State::HEAPIFY_DOWN;
        stepClock.restart();
    }

    void toggleType() {
        isMinHeap = !isMinHeap;
        statusMsg = (isMinHeap ? "Switched to Min-Heap" : "Switched to Max-Heap");
        data.clear();
        animations.clear();
        state = State::IDLE;
    }

    void triggerHeapify() {
        if (state != State::IDLE || data.empty()) return;
        // Simple logic: if currentIdx was set to 0 by deleteRoot, heapify down.
        // Otherwise, heapify up from the last element.
        if (currentIdx == 0 && data.size() > 1) {
            state = State::HEAPIFY_DOWN;
        } else {
            currentIdx = data.size() - 1;
            state = State::HEAPIFY_UP;
        }
        stepClock.restart();
    }

    void updatePositions(int index, float x, float y, float xOffset) {
        if (index >= data.size()) return;
        animations[index].targetPos = sf::Vector2f(x, y);
        animations[index].currentPos.x += (animations[index].targetPos.x - animations[index].currentPos.x) * 0.1f;
        animations[index].currentPos.y += (animations[index].targetPos.y - animations[index].currentPos.y) * 0.1f;

        updatePositions(2 * index + 1, x - xOffset, y + 80, xOffset * 0.5f);
        updatePositions(2 * index + 2, x + xOffset, y + 80, xOffset * 0.5f);
    }

    void update() {
        if (state != State::IDLE && stepClock.getElapsedTime().asSeconds() > stepInterval) {
            if (state == State::HEAPIFY_UP) processHeapifyUp();
            else if (state == State::HEAPIFY_DOWN) processHeapifyDown();
            stepClock.restart();
        }

        if (!data.empty()) {
            updatePositions(0, 600, 150, 300);
        }
    }

    void draw(sf::RenderWindow& window) {
        if (font) {
            sf::Text text(*font, statusMsg, 20);
            text.setFillColor(sf::Color(241, 196, 15));
            text.setPosition({10, 50});
            window.draw(text);
        }

        // Draw edges
        for (int i = 0; i < data.size(); ++i) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            if (left < data.size()) {
                sf::Vertex line[] = { sf::Vertex{animations[i].currentPos, sf::Color(189, 195, 199)}, sf::Vertex{animations[left].currentPos, sf::Color(189, 195, 199)} };
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
            if (right < data.size()) {
                sf::Vertex line[] = { sf::Vertex{animations[i].currentPos, sf::Color(189, 195, 199)}, sf::Vertex{animations[right].currentPos, sf::Color(189, 195, 199)} };
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }

        // Draw nodes
        for (int i = 0; i < data.size(); ++i) {
            sf::CircleShape circle(22);
            circle.setOrigin({22, 22});
            circle.setPosition(animations[i].currentPos);
            
            sf::Color fillCol = sf::Color(52, 152, 219); // Standard Blue
            if (i == currentIdx && state != State::IDLE) fillCol = sf::Color(231, 76, 60); // Highlight Red
            
            circle.setFillColor(fillCol);
            circle.setOutlineThickness(2.f);
            circle.setOutlineColor(sf::Color::White);
            window.draw(circle);

            if (font) {
                sf::Text text(*font, std::to_string(data[i]), 18);
                text.setFillColor(sf::Color::White);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setOrigin({bounds.size.x / 2.0f + bounds.position.x, bounds.size.y / 2.0f + bounds.position.y});
                text.setPosition(animations[i].currentPos);
                window.draw(text);
            }
        }
    }
};
