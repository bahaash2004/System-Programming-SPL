# 🧮 Linear Algebra Engine (LAE)

A high-performance, concurrent mathematical engine for evaluating complex linear algebra expressions. 

This project implements a custom, fine-grained multi-threading architecture to resolve nested matrix operations (Addition, Multiplication, Transposition, Negation) provided via a JSON Abstract Syntax Tree (AST).

## ✨ Key Features

* **Custom Fatigue-Based Thread Pool (`TiredExecutor`):** Implements a unique load-balancing algorithm. Worker threads (`TiredThread`) accumulate "fatigue" based on their execution time and a personal fatigue factor. The executor always assigns new tasks to the least fatigued worker using a `PriorityBlockingQueue` (Min-Heap).
* **Fine-Grained Synchronization:** * Replaces coarse-grained `synchronized` blocks with `ReentrantReadWriteLock` in shared memory components (`SharedVector`). This allows multiple threads to read matrix data concurrently without blocking, while ensuring exclusive access during state-mutating operations like matrix transposition.
  * Uses `wait()` / `notifyAll()` mechanisms to synchronize the main thread with worker completion, strictly avoiding CPU-intensive busy-waiting.
* **Cache-Locality Optimization:** During matrix multiplication, the right-hand operand is intelligently loaded into memory as **Column-Major**, transforming the complex matrix multiplication into highly efficient, cache-friendly vector dot products.
* **Graceful Shutdown:** Utilizes a `POISON_PILL` design pattern to ensure all worker threads terminate cleanly after completing their assigned tasks, preventing memory leaks and zombie threads.
* **Dynamic Tree Resolution:** Converts complex mathematical expressions (e.g., `A + B + C`) into strict binary trees and iteratively resolves them bottom-up.

## 🏗️ Architecture & Core Components

1. **`LinearAlgebraEngine` (The Orchestrator):**
   Parses the computation tree, identifies resolvable nodes (nodes where all children are evaluated matrices), and breaks down the mathematical operation into atomic `Runnable` tasks (e.g., computing a single cell in the result matrix).
2. **`TiredExecutor` & `TiredThread` (The Concurrency Engine):**
   Instead of using standard Java ThreadPools, this project features a bespoke executor. Workers receive tasks via an internal `ArrayBlockingQueue` (size 1) to block efficiently when idle. The executor delegates tasks wrapped in lifecycle-hooks to ensure workers are safely returned to the priority pool upon completion.
3. **`SharedMatrix` & `SharedVector` (Thread-Safe Memory):**
   Encapsulates standard 2D arrays with Read/Write locks, ensuring thread-safe concurrent access without compromising performance during heavy read-bound operations.

## 🚀 Getting Started

### Prerequisites
* Java Development Kit (JDK) 8 or higher.

### Input Format
The engine accepts a JSON file representing the mathematical expression tree. 

**Example (`input.json`):** Computing $(A + B) \times C$
```json
{
  "operator": "*",
  "operands": [
    {
      "operator": "+",
      "operands": [
        {"matrix": [[1, 2], [3, 4]]},
        {"matrix": [[5, 6], [7, 8]]}
      ]
    },
    {"matrix": [[1, 0], [0, 1]]}
  ]
}
