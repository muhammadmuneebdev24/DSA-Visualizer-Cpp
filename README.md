# Algorithm Visualizer (SFML)

A clean, beginner-friendly C++ application built with SFML to visualize fundamental data structures and algorithms. This project focuses on simplicity and clear step-by-step logic.

## 🚀 Features & Usage

### 1. AVL Tree (Balanced BST)
Visualize how a Binary Search Tree maintains its balance through rotations.
- **How to use**:
  - Press **'I'** to insert a random number.
  - The tree will automatically detect if it's unbalanced.
  - Press **'Y'** to confirm and perform the necessary rotation (LL, RR, LR, RL).
  - Status messages explain the balance factor of each node.

### 2. Min & Max Heap
Understand array-based heaps with manual heapify-up and heapify-down operations.
- **How to use**:
  - Press **'I'** to add a node to the end of the array.
  - Press **'H'** to start the **Heapify** process step-by-step.
  - Press **'D'** to move the last element to the root (for deletion) and press **'H'** to heapify-down.
  - Press **'T'** to toggle between **Min-Heap** and **Max-Heap** modes.
  - The tool explains exactly **why** each swap is happening in real-time.

### 3. Minimum Spanning Tree (MST)
Watch Kruskal's and Prim's algorithms build the most efficient network connection.
- **How to use**:
  - **Left-Click** to add nodes to the canvas.
  - **Right-Click** two nodes to connect them with an edge.
  - Press **'K'** for **Kruskal's Algorithm** (starts with smallest edges).
  - Press **'P'** for **Prim's Algorithm** (grows from Node 0).
  - The original graph fades away, showing only the "new graph" (the MST) as it is built.
  - Press **'C'** to clear the canvas.

### 4. Shortest Path (Dijkstra)
Find the most efficient path between any two nodes.
- **How to use**:
  - Build your graph using **Left-Click** (nodes) and **Right-Click** (edges).
  - Press **'S'** to start the algorithm from Node 0.
  - The system will ask you to **Left-Click a Goal Node**.
  - Once selected, the visualizer draws only the final shortest path to that goal.
  - Press **'C'** to clear and reset.

### 5. Graph Traversal (BFS & DFS)
Explore how Breadth-First and Depth-First searches visit nodes in a graph.
- **How to use**:
  - Build your graph manually.
  - Press **'B'** to start **BFS** (Level-by-level).
  - Press **'D'** to start **DFS** (Depth exploration).

---

## 🛠️ Requirements
- **Compiler**: C++17 or higher (g++ recommended).
- **Library**: SFML (Graphics, Window, System).

## 🔨 How to Compile
Ensure SFML is installed on your system. Run the following command in your terminal:

```bash
g++ main.cpp -o visualizer -lsfml-graphics -lsfml-window -lsfml-system
```

## 🎮 Controls Summary
| Key | Action |
|-----|--------|
| **1 - 5** | Switch between Algorithm Modes |
| **Esc** | Return to Main Menu |
| **L-Click** | Add Node / Set Goal |
| **R-Click** | Connect Nodes |
| **C** | Clear / Reset Canvas |
| **H** | Trigger Heapify / BFS-DFS Step |
| **S** | Start Shortest Path |
| **K / P** | Start Kruskal / Prim MST |

---
*Created for beginners to learn algorithms visually and simply.*
