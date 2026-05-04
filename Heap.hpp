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
    bool typeSelected = false;
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

    enum class Task { NONE, UP, DOWN };
    Task pendingTask = Task::NONE;

    void processHeapifyUp() {
        if (currentIdx <= 0) {
            state = State::IDLE;
            pendingTask = Task::NONE;
            statusMsg = "Insertion complete. Press 'I' to add node or 'D' to delete.";
            return;
        }

        int parent = (currentIdx - 1) / 2;
        bool shouldSwap = isMinHeap ? (data[currentIdx] < data[parent]) : (data[currentIdx] > data[parent]);

        if (shouldSwap) {
            statusMsg = "Swapping " + std::to_string(data[currentIdx]) + " and " + std::to_string(data[parent]) + ".";
            std::swap(data[currentIdx], data[parent]);
            currentIdx = parent;
        } else {
            state = State::IDLE;
            pendingTask = Task::NONE;
            statusMsg = "Heap property satisfied. Press 'I' to add node.";
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
            statusMsg = "Swapping " + std::to_string(data[currentIdx]) + " with " + std::to_string(data[target]) + ".";
            std::swap(data[currentIdx], data[target]);
            currentIdx = target;
        } else {
            state = State::IDLE;
            pendingTask = Task::NONE;
            statusMsg = "Deletion/Heapify-down complete. Press 'I' to add node.";
        }
    }

public:
    Heap(sf::Font* f) : font(f) {
        statusMsg = "Choose Type: Press '1' for Max Heap, '2' for Min Heap";
    }

    void selectType(bool minHeap) {
        isMinHeap = minHeap;
        typeSelected = true;
        statusMsg = (isMinHeap ? "Min Heap Selected. Press 'I' to add node." : "Max Heap Selected. Press 'I' to add node.");
        data.clear();
        animations.clear();
        pendingTask = Task::NONE;
    }

    bool isTypeSelected() const { return typeSelected; }

    void insert(int key) {
        if (!typeSelected) return;
        data.push_back(key);
        NodeAnim anim;
        anim.currentPos = sf::Vector2f(600, 700);
        animations.push_back(anim);
        currentIdx = data.size() - 1;
        pendingTask = Task::UP;
        statusMsg = "Added " + std::to_string(key) + ". Heapify is required! Press 'H' to do it.";
    }

    void deleteRoot() {
        if (data.empty() || state != State::IDLE || !typeSelected) return;
        if (data.size() == 1) {
            data.pop_back();
            animations.pop_back();
            statusMsg = "Heap is now empty.";
            return;
        }
        statusMsg = "Root removed. Heapify is required! Press 'H' to do it.";
        data[0] = data.back();
        data.pop_back();
        animations.pop_back();
        currentIdx = 0;
        pendingTask = Task::DOWN;
    }

    void toggleType() {
        typeSelected = false;
        statusMsg = "Choose Type: Press '1' for Max Heap, '2' for Min Heap";
        data.clear();
        animations.clear();
        state = State::IDLE;
        pendingTask = Task::NONE;
    }

    void triggerHeapify() {
        if (!typeSelected || state != State::IDLE || data.empty() || pendingTask == Task::NONE) return;
        if (pendingTask == Task::DOWN) {
            state = State::HEAPIFY_DOWN;
        } else if (pendingTask == Task::UP) {
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
        if (!typeSelected) return;
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
            sf::Text text(*font, statusMsg, 24);
            text.setFillColor(sf::Color::White);
            text.setPosition({10, 80});
            window.draw(text);
        }

        if (!typeSelected) return;

        // Draw edges
        for (int i = 0; i < data.size(); ++i) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            if (left < data.size()) {
                sf::Vertex line[] = { sf::Vertex{animations[i].currentPos, sf::Color::White}, sf::Vertex{animations[left].currentPos, sf::Color::White} };
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
            if (right < data.size()) {
                sf::Vertex line[] = { sf::Vertex{animations[i].currentPos, sf::Color::White}, sf::Vertex{animations[right].currentPos, sf::Color::White} };
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }

        // Draw nodes
        for (int i = 0; i < data.size(); ++i) {
            sf::CircleShape circle(25);
            circle.setOrigin({25, 25});
            circle.setPosition(animations[i].currentPos);
            
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineThickness(2.f);
            circle.setOutlineColor(sf::Color::White);
            window.draw(circle);

            if (font) {
                sf::Text text(*font, std::to_string(data[i]), 20);
                text.setFillColor(sf::Color::White);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setOrigin({bounds.size.x / 2.0f + bounds.position.x, bounds.size.y / 2.0f + bounds.position.y});
                text.setPosition(animations[i].currentPos);
                window.draw(text);
            }
        }
    }
};
