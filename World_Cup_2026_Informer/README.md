# 🚀 STOMP Protocol Client-Server Application

**Systems Programming (SPL) - Assignment 3**

A robust, highly concurrent Client-Server messaging application implementing the **STOMP (Simple Text Oriented Messaging Protocol)**. The system features a custom-built Java server (supporting both Thread-Per-Client and Reactor patterns), a C++ client, and a Python-based SQLite database for data persistence.

---

## ✨ Key Features

* **Pub/Sub Architecture:** Clients can subscribe to topics (channels), send messages to topics, and receive real-time updates from the server.
* **Dual Server Patterns:** * `Thread-Per-Client (TPC)`: For dedicated, blocking client handling (1 thread per client).
  * `Reactor`: A highly scalable, non-blocking I/O server utilizing the Java `Selector` and a generic `ActorThreadPool` to handle thousands of concurrent connections efficiently.
* **Data Persistence:** A Python microservice running an SQLite database ensures user credentials, login states, and history survive server restarts.
* **Thread Safety:** Comprehensive use of synchronization, `ReadWriteLock`, and concurrent data structures to prevent race conditions, deadlocks, and message reordering.
* **Graceful Disconnect:** Implements receipt-based acknowledgment to ensure safe and complete client disconnections without data loss.

---

## 🏗️ System Architecture



The project is seamlessly divided into three main components working in harmony:

1. **Java Server (`/server`)**: Acts as the central STOMP Broker. It parses incoming frames, manages topic subscriptions, and efficiently broadcasts messages to active subscribers.
2. **C++ Client (`/client`)**: The user-facing interface. It interacts with the user via CLI, parses complex game-event JSON files, and communicates asynchronously with the server via TCP sockets.
3. **Python Database (`/data`)**: A dedicated microservice that listens on a local socket to securely execute SQL queries requested by the Java server.

---

## 🛠️ Prerequisites & Technologies

* **Java:** JDK 11 or higher & Maven.
* **C++:** g++ compiler (supporting C++11/14), Boost libraries (`libboost-system`, `libboost-thread`), and Make.
* **Python:** Python 3.x (with built-in `sqlite3`).

---

## 🚀 How to Build and Run

To get the system running, you need to open three separate terminal windows and follow these steps in order:

### Step 1: Start the Database Server
The database microservice must be running *before* the Java server starts.
```bash
cd data
python3 sql_server.py
```
Keep this terminal open in the background. You should see: [STOMP_PYTHON_SQL_SERVER] Listening on 127.0.0.1:7778

Step 2: Build & Run the Java Server
Open a new terminal to compile and launch the broker. You can choose to run it as either a tpc (Thread-Per-Client) or reactor server.

```Bash
cd server
mvn clean compile
```
# Usage: mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="<port> <tpc/reactor>"
mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 reactor"
Step 3: Build & Run the C++ Client
Open a third terminal to compile and start the interactive client.

```Bash
cd client
make clean
make
```
# Usage: ./bin/StompWCIClient <IP> <Port>
./bin/StompWCIClient 127.0.0.1 7777
💻 Usage & Commands (Client Interface)
Once the C++ client is running, you can use the following commands to interact with the system:

1. Login to the server:

```Bash
login 127.0.0.1:7777 <username> <password>
# Example: login 127.0.0.1:7777 bahaa 1234
2. Join a channel (Subscribe):
```
```Bash
join <channel_name>
# Example: join Germany_Japan
3. Report events from a JSON file:
Parses a JSON file containing game events and sends them to the subscribed channel.
```
```Bash
report data/events1.json
4. Leave a channel (Unsubscribe):
```
```Bash
exit <channel_name>
# Example: exit Germany_Japan
5. Logout and Disconnect:
Safely disconnects from the server, waiting for a server receipt before closing the TCP socket.
```
```Bash
logout
```
📡 STOMP Protocol Implementation
The system fully implements a functional subset of the STOMP protocol for internal client-server communication. The underlying frames exchanged over TCP include:

Client-to-Server Frames:
CONNECT: Initiates a session with the server (includes login and passcode headers).

SUBSCRIBE: Registers the client to a specific topic to start receiving messages.

UNSUBSCRIBE: Removes the client from a specific topic.

SEND: Publishes a new message payload to a designated topic.

DISCONNECT: Safely terminates the session, requiring a receipt-id for a graceful shutdown.

Server-to-Client Frames:
CONNECTED: Acknowledges a successful login and session creation.

MESSAGE: Broadcasts a published message to all clients currently subscribed to the relevant topic.

RECEIPT: Confirms that a client's specific request (e.g., Subscribe, Disconnect) was successfully processed.

ERROR: Sent when a protocol violation occurs (e.g., wrong password, sending without a subscription), immediately followed by connection termination.

🧠 Design Highlights
Dependency Injection: The server utilizes abstract factories to inject specific protocol and encoder/decoder logic, keeping the generic Reactor/TPC networking code completely decoupled and clean (Open/Closed Principle).

Actor Thread Pool (Reactor): Work is dispatched to an ActorThreadPool, which utilizes a locking mechanism (synchronized blocks and WeakHashMap) to ensure that a single client's messages are strictly handled by one thread at a time. This prevents message reordering while maintaining massive concurrency.

Concurrency Handling: ConcurrentHashMap and ConcurrentLinkedQueue are used extensively throughout the Connections interface to manage active clients and dynamic topic subscriptions safely, minimizing unnecessary locking overhead.

Developed as part of the Systems Programming course at Ben-Gurion University of the Negev.
