#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>

struct AVLNode {
    int key;
    int height;
    AVLNode* left;
    AVLNode* right;
    sf::Vector2f targetPos;
    sf::Vector2f currentPos;

    AVLNode(int k, sf::Vector2f startPos) : key(k), height(1),
     left(nullptr), right(nullptr), targetPos(startPos),
      currentPos(startPos) {}
};

class AVLTree {
private:
    AVLNode* root = nullptr;
    sf::Font* font;
    std::string statusMsg = "AVL Tree: Press 'I' to insert node";
    bool awaitingRotation = false;
    int lastInsertedKey = -1;

    int height(AVLNode* N) { return N ? N->height : 0; }
    int getBalance(AVLNode* N) { return N ? height(N->left) - height(N->right) : 0; }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = std::max(height(y->left), height(y->right)) + 1;
        x->height = std::max(height(x->left), height(x->right)) + 1;
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = std::max(height(x->left), height(x->right)) + 1;
        y->height = std::max(height(y->left), height(y->right)) + 1;
        return y;
    }

    AVLNode* insertNode(AVLNode* node, int key, sf::Vector2f startPos) {
        if (!node) return new AVLNode(key, startPos);
        if (key < node->key) node->left = insertNode(node->left, key, startPos);
        else if (key > node->key) node->right = insertNode(node->right, key, startPos);
        else return node;

        node->height = 1 + std::max(height(node->left), height(node->right));
        int balance = getBalance(node);
        
        // Only detect the first imbalance if we aren't already waiting for a rotation
        if (!awaitingRotation && (balance > 1 || balance < -1)) {
            awaitingRotation = true;
            statusMsg = "Imbalance at node " + std::to_string(node->key) + " (BF: " + std::to_string(balance) + "). Perform rotation? (Press 'Y')";
        }

        return node;
    }

    AVLNode* balanceSubtree(AVLNode* node, int key) {
        if (!node) return nullptr;

        // Recursive balance (standard AVL logic but triggered manually)
        node->left = balanceSubtree(node->left, key);
        node->right = balanceSubtree(node->right, key);

        node->height = 1 + std::max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && key < node->left->key) { 
            statusMsg = "Performed LL (Right) Rotation at " + std::to_string(node->key); 
            return rightRotate(node); 
        }
        if (balance < -1 && key > node->right->key) { 
            statusMsg = "Performed RR (Left) Rotation at " + std::to_string(node->key); 
            return leftRotate(node); 
        }
        if (balance > 1 && key > node->left->key) { 
            statusMsg = "Performed LR (Left-Right) Rotation at " + std::to_string(node->key); 
            node->left = leftRotate(node->left); 
            return rightRotate(node); 
        }
        if (balance < -1 && key < node->right->key) { 
            statusMsg = "Performed RL (Right-Left) Rotation at " + std::to_string(node->key); 
            node->right = rightRotate(node->right); 
            return leftRotate(node); 
        }

        return node;
    }

    void updatePositions(AVLNode* node, float x, float y, float xOffset) {
        if (!node) return;
        node->targetPos = sf::Vector2f(x, y);
        node->currentPos.x += (node->targetPos.x - node->currentPos.x) * 0.05f;
        node->currentPos.y += (node->targetPos.y - node->currentPos.y) * 0.05f;
        updatePositions(node->left, x - xOffset, y + 80, xOffset * 0.5f);
        updatePositions(node->right, x + xOffset, y + 80, xOffset * 0.5f);
    }

    void drawNode(sf::RenderWindow& window, AVLNode* node) {
        if (!node) return;
        
        if (node->left) {
            sf::Vertex line[] = { sf::Vertex{node->currentPos, sf::Color(189, 195, 199)}, sf::Vertex{node->left->currentPos, sf::Color(189, 195, 199)} };
            window.draw(line, 2, sf::PrimitiveType::Lines); 
        }
        if (node->right) {
            sf::Vertex line[] = { sf::Vertex{node->currentPos, sf::Color(189, 195, 199)}, sf::Vertex{node->right->currentPos, sf::Color(189, 195, 199)} };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        sf::CircleShape shadow(20);
        shadow.setOrigin({20, 20});
        shadow.setPosition(node->currentPos + sf::Vector2f(3.f, 3.f));
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(shadow);

        sf::CircleShape circle(20);
        circle.setOrigin({20, 20});
        circle.setPosition(node->currentPos);
        circle.setFillColor(sf::Color(52, 152, 219));
        circle.setOutlineThickness(3.f);
        circle.setOutlineColor(sf::Color(41, 128, 185));
        window.draw(circle);

        if (font) {
            sf::Text text(*font, std::to_string(node->key), 16);
            text.setFillColor(sf::Color::White);
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin({bounds.size.x / 2.0f + bounds.position.x, bounds.size.y / 2.0f + bounds.position.y});
            text.setPosition(node->currentPos);
            window.draw(text);

            // Display Balance Factor
            int bf = getBalance(node);
            sf::Text bfText(*font, "BF: " + std::to_string(bf), 12);
            bfText.setFillColor(sf::Color(231, 76, 60)); // Red color for BF
            bfText.setPosition(node->currentPos + sf::Vector2f(22.f, -25.f));
            window.draw(bfText);
        }

        drawNode(window, node->left);
        drawNode(window, node->right);
    }

public:
    AVLTree(sf::Font* f) : font(f) {}
    
    void insert(int key) {
        if (awaitingRotation) {
            statusMsg = "Please confirm the current rotation first! (Press 'Y')";
            return;
        }
        lastInsertedKey = key;
        statusMsg = "Inserted " + std::to_string(key) + ". Calculating balance factors...";
        root = insertNode(root, key, sf::Vector2f(600, 150));
    }

    void confirmRotation() {
        if (awaitingRotation) {
            root = balanceSubtree(root, lastInsertedKey);
            awaitingRotation = false;
        }
    }

    void update() {
        updatePositions(root, 600, 150, 300);
    }

    void draw(sf::RenderWindow& window) {
        if (font) {
            // Draw a subtle background for the status message
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
        }
        drawNode(window, root);
    }
};
