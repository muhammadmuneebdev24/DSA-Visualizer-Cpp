#include "AVLTree.hpp"
#include "GraphTraversal.hpp"
#include "Heap.hpp"
#include "MST.hpp"
#include "ShortestPath.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

// Utility to handle SFML 2 vs SFML 3 event polling differences loosely
// We will mainly use realtime input (sf::Keyboard, sf::Mouse) for simplicity

int main() {
  sf::RenderWindow window(sf::VideoMode({1200, 800}), "Algorithm Visualizer");
  window.setFramerateLimit(60);

  sf::Font font;
  // SFML 3 uses openFromFile
  if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
    std::cerr
        << "Could not load font. Please ensure arial.ttf is in the directory."
        << std::endl;
  }

  AVLTree avl(&font);
  Heap heap(&font);
  GraphTraversal gt(&font);
  MST mst(&font);
  ShortestPath sp(&font);

  int currentMode = 0; // 0: Menu, 1: AVL, 2: Heap, 3: Graph, 4: MST, 5: Shortest Path

  std::vector<std::string> options = {
      "1. AVL Tree",
      "2. Max/Min Heap",
      "3. Graph Traversal (BFS/DFS)",
      "4. Minimum Spanning Tree",
      "5. Shortest Path (Dijkstra)"
  };

  std::vector<sf::RectangleShape> btnShadows;
  std::vector<sf::RectangleShape> btnRects;
  std::vector<sf::Text> btnTexts;
  for(int i=0; i<5; ++i) {
      sf::RectangleShape shadow({450.f, 60.f});
      shadow.setPosition({375.f + 5.f, 250.f + i * 80.f + 5.f});
      shadow.setFillColor(sf::Color(0, 0, 0, 100));
      btnShadows.push_back(shadow);

      sf::RectangleShape rect({450.f, 60.f});
      rect.setPosition({375.f, 250.f + i * 80.f});
      rect.setFillColor(sf::Color(44, 49, 58));
      rect.setOutlineColor(sf::Color(97, 175, 239));
      rect.setOutlineThickness(2.f);
      btnRects.push_back(rect);

      sf::Text t(font, options[i], 24);
      t.setFillColor(sf::Color::White);
      t.setPosition({400.f, 265.f + i * 80.f});
      btnTexts.push_back(t);
  }

  sf::Text title(font, "Algorithm Visualizer", 48);
  title.setFillColor(sf::Color(229, 192, 123));
  title.setPosition({375.f, 100.f});

  sf::Text subtitle(font, "Select an algorithm to start or press Escape to go back to Menu", 20);
  subtitle.setFillColor(sf::Color(171, 178, 191));
  subtitle.setPosition({320.f, 170.f});

  sf::Vector2f currentMousePos;

  while (window.isOpen()) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        currentMousePos = window.mapPixelToCoords(mouseMoved->position);
      } else if (const auto *keyPressed =
                     event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          if (currentMode == 0) window.close();
          else currentMode = 0;
        } else if (keyPressed->code == sf::Keyboard::Key::Num1) {
          if (currentMode == 2 && !heap.isTypeSelected()) heap.selectType(false); // Max Heap
          else currentMode = 1;
        } else if (keyPressed->code == sf::Keyboard::Key::Num2) {
          if (currentMode == 2 && !heap.isTypeSelected()) heap.selectType(true); // Min Heap
          else {
              currentMode = 2;
              heap.toggleType();
          }
        } else if (keyPressed->code == sf::Keyboard::Key::Num3) {
          currentMode = 3;
        } else if (keyPressed->code == sf::Keyboard::Key::Num4) {
          currentMode = 4;
        } else if (keyPressed->code == sf::Keyboard::Key::Num5) {
          currentMode = 5;
        } else if (keyPressed->code == sf::Keyboard::Key::I) {
          if (currentMode == 1)
            avl.insert(rand() % 100);
          if (currentMode == 2)
            heap.insert(rand() % 100);
        } else if (keyPressed->code == sf::Keyboard::Key::D) {
          if (currentMode == 2)
            heap.deleteRoot();
          if (currentMode == 3)
            gt.startDFS();
        } else if (keyPressed->code == sf::Keyboard::Key::B) {
          if (currentMode == 3)
            gt.startBFS();
        } else if (keyPressed->code == sf::Keyboard::Key::K) {
          if (currentMode == 4)
            mst.startKruskal();
        } else if (keyPressed->code == sf::Keyboard::Key::P) {
          if (currentMode == 4)
            mst.startPrim();
        } else if (keyPressed->code == sf::Keyboard::Key::S) {
          if (currentMode == 5)
            sp.startDijkstra();
        } else if (keyPressed->code == sf::Keyboard::Key::Y) {
          if (currentMode == 1)
            avl.confirmRotation();
        } else if (keyPressed->code == sf::Keyboard::Key::H) {
          if (currentMode == 2)
            heap.triggerHeapify();
        } else if (keyPressed->code == sf::Keyboard::Key::T) {
          if (currentMode == 2)
            heap.toggleType();
        } else if (keyPressed->code == sf::Keyboard::Key::C) {
          if (currentMode == 4)
            mst.clear();
          if (currentMode == 5)
            sp.clear();
        }
      } else if (const auto *mousePressed =
                     event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f pos = window.mapPixelToCoords(mousePressed->position);
        if (mousePressed->button == sf::Mouse::Button::Left) {
          if (currentMode == 0) {
            for(size_t i=0; i<btnRects.size(); ++i) {
                if (btnRects[i].getGlobalBounds().contains(pos)) {
                    currentMode = i + 1;
                    if (currentMode == 2) heap.toggleType();
                }
            }
          } else if (currentMode == 3)
            gt.addNode(pos);
          else if (currentMode == 4)
            mst.addNode(pos);
          else if (currentMode == 5)
            sp.addNode(pos);
        } else if (mousePressed->button == sf::Mouse::Button::Right) {
          if (currentMode == 3)
            gt.handleRightClick(pos);
          if (currentMode == 4)
            mst.handleRightClick(pos);
          if (currentMode == 5)
            sp.handleRightClick(pos);
        }
      }
    }

    // Update logic
    if (currentMode == 1)
      avl.update();
    if (currentMode == 2)
      heap.update();
    if (currentMode == 3)
      gt.update();
    if (currentMode == 4)
      mst.update();
    if (currentMode == 5)
      sp.update();

    // Draw logic
    if (currentMode == 0) {
      window.clear(sf::Color(30, 34, 40));
      window.draw(title);
      window.draw(subtitle);
      
      for(size_t i=0; i<btnRects.size(); ++i) {
          if (btnRects[i].getGlobalBounds().contains(currentMousePos)) {
              btnRects[i].setFillColor(sf::Color(75, 82, 99)); // Hover color
              btnRects[i].setOutlineColor(sf::Color(152, 195, 121)); // Hover outline
          } else {
              btnRects[i].setFillColor(sf::Color(44, 49, 58)); // Normal color
              btnRects[i].setOutlineColor(sf::Color(97, 175, 239)); // Normal outline
          }
          window.draw(btnShadows[i]);
          window.draw(btnRects[i]);
          window.draw(btnTexts[i]);
      }
    } else {
      window.clear(sf::Color(35, 39, 46)); // Nicer dark gray instead of pure black

      // Draw Menu
      sf::Text menuText(font,
                        "Press Esc to return to Main Menu  |  1: AVL  |  2: Heap  |  3: BFS/DFS  |  4: MST  |  5: Shortest Path",
                        18);
      menuText.setFillColor(sf::Color(171, 178, 191));
      menuText.setPosition({10, 10});
      window.draw(menuText);

      // Draw active visualization
      if (currentMode == 1)
        avl.draw(window);
      else if (currentMode == 2)
        heap.draw(window);
      else if (currentMode == 3)
        gt.draw(window);
      else if (currentMode == 4)
        mst.draw(window);
      else if (currentMode == 5)
        sp.draw(window);
    }

    window.display();
  }

  return 0;
}
