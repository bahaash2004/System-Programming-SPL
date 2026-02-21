# 🧮 Linear Algebra Engine (LAE)

A high-performance, concurrent mathematical engine developed in **Java** for evaluating complex, nested linear algebra expressions. 

This project was developed as part of an advanced System Programming and Concurrency course at **Ben-Gurion University of the Negev**. It implements a custom, fine-grained multi-threading architecture to parse, optimize, and resolve matrix operations (Addition, Multiplication, Transposition, Negation) provided via a JSON Abstract Syntax Tree (AST).

---

## ✨ Key Architectural Features

### 1. Smart Load Balancing (`TiredExecutor`)
Instead of relying on standard Java `ThreadPoolExecutor`, this system features a custom fatigue-based scheduling algorithm:
* **Fatigue Tracking:** Worker threads (`TiredThread`) measure their own execution time. This time is multiplied by a randomized "fatigue factor" to calculate their current fatigue level.
* **Min-Heap Delegation:** The executor maintains a `PriorityBlockingQueue` of idle workers, automatically sorting them so the least fatigued worker is always at the top of the queue.
* **Zero Busy-Waiting:** Workers use an internal `ArrayBlockingQueue` (size 1) for task handoffs, allowing them to block natively (`take()`) and consume zero CPU cycles while waiting for tasks.

### 2. Fine-Grained Synchronization & Thread Safety
The system utilizes advanced locking mechanisms to maximize throughput and prevent race conditions:
* **`ReentrantReadWriteLock`:** Shared memory components (`SharedVector`) replace coarse `synchronized` methods with Read/Write locks. This allows thousands of threads to read matrix rows/columns simultaneously during operations like Matrix Multiplication, while ensuring exclusive access during state-mutating operations like `TRANSPOSE`.
* **Wait/Notify All:** The orchestrator strictly avoids active polling (busy-wait). It uses `wait()` and `notifyAll()` within `synchronized` blocks to halt the main thread precisely until all in-flight worker tasks are completed.
* **Atomic Variables:** Utilizes `AtomicInteger` and `AtomicBoolean` for thread-safe state tracking (e.g., tracking `inFlight` tasks) without the overhead of heavy locking.

### 3. Cache-Locality Optimization
Matrix multiplication is notoriously memory-intensive. To optimize CPU cache hit rates:
* The engine dynamically identifies the right-hand operand in a multiplication node and pre-loads it into shared memory in **Column-Major** order.
* This transforms the complex $O(n^3)$ cell calculations into highly efficient, contiguous vector dot-products (`vecMatMul`), significantly reducing Cache Misses.

### 4. Graceful Shutdown & Poison Pill
The system ensures memory safety and clean thread termination:
* Once the computation tree is fully resolved, the executor broadcasts a `POISON_PILL` task to all workers.
* Workers cleanly break their lifecycle loops upon ingesting the pill, ensuring no zombie threads are left running in the background.

---

## 🏗️ System Flow (The Orchestrator)

1. **Input Parsing:** The `InputParser` reads a JSON file and constructs an Abstract Syntax Tree (AST) of `ComputationNode` objects.
2. **Pre-processing:** The `associativeNesting()` function flattens complex linear expressions (e.g., `A + B + C`) into strict binary sub-trees `((A + B) + C)`.
3. **Iterative Resolution:** The `LinearAlgebraEngine` acts as the orchestrator. It iteratively scans the tree for "resolvable" nodes (operations where all children are fully evaluated matrices).
4. **Task Granularity:** Once a resolvable node is found, the engine breaks the mathematical operation down into atomic `Runnable` tasks (e.g., one task per row/cell in the result matrix) and submits them to the `TiredExecutor`.
5. **Tree Collapse:** Computed results are packaged back into matrix nodes, replacing the operation nodes in the tree, until the entire tree collapses into a single final matrix.

---

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
