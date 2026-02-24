# STOMP Protocol Client-Server Application 🚀

**Systems Programming (SPL) - Assignment 3**

A robust, highly concurrent Client-Server messaging application implementing the **STOMP (Simple Text Oriented Messaging Protocol)**. The system features a custom-built Java server (supporting both Thread-Per-Client and Reactor patterns), a C++ client, and a Python-based SQLite database for data persistence.

---

## 🌟 Key Features

* **Pub/Sub Architecture:** Clients can subscribe to topics (channels), send messages to topics, and receive real-time updates.
* **Dual Server Patterns:** * `Thread-Per-Client (TPC)`: For dedicated, blocking client handling.
  * `Reactor`: A highly scalable, non-blocking I/O server utilizing the `Selector` and a generic `ActorThreadPool` to handle thousands of concurrent connections efficiently.
* **Data Persistence:** A Python microservice running an SQLite database ensures user credentials and login history survive server restarts.
* **Thread Safety:** Comprehensive use of synchronization, `ReadWriteLock`, and concurrent data structures to prevent race conditions and deadlocks.
* **Graceful Disconnect:** Implements receipt-based acknowledgment to ensure safe and complete client disconnections.

---

## 🏗️ System Architecture

The project is divided into three main components:
1. **Java Server (`/server`)**: Acts as the STOMP Broker. Parses frames, manages subscriptions, and broadcasts messages.
2. **C++ Client (`/client`)**: Interacts with the user, parses game-event JSON files, and communicates with the server via sockets.
3. **Python Database (`/data`)**: A microservice that listens on a local socket to execute SQL queries requested by the Java server.

---

## 🛠️ Prerequisites & Technologies

* **Java:** JDK 11 or higher & Maven.
* **C++:** g++ compiler (supporting C++11/14), Boost libraries (`libboost-system`, `libboost-thread`), and Make.
* **Python:** Python 3.x.

---

## 🚀 How to Build and Run

### Step 1: Start the Database Server
The database must be running *before* the Java server starts.
```bash
cd data
python3 sql_server.py
# Keep this terminal open. You should see: [STOMP_PYTHON_SQL_SERVER] Listening on 127.0.0.1:7778
```

### Step 2: Build & Run the Java Server
Open a new terminal.
```bash
cd server
mvn clean compile
```
Run the server. You can choose either `tpc` (Thread-Per-Client) or `reactor`.
```bash
# Usage: mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="<port> <tpc/reactor>"
mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 reactor"
```

### Step 3: Build & Run the C++ Client
Open a third terminal.
```bash
cd client
make clean
make
```
Run the client:
```bash
# Usage: ./bin/StompWCIClient <IP> <Port>
./bin/StompWCIClient 127.0.0.1 7777
```

---

## 💻 Usage & Commands (Client Interface)

Once the C++ client is running, you can use the following commands:

**1. Login to the server:**
```bash
login <IP>:<Port> <username> <password>
# Example: login 127.0.0.1:7777 bahaa 1234
```

**2. Join a channel (Subscribe):**
```bash
join <channel_name>
# Example: join Germany_Japan
```

**3. Report events from a JSON file:**
Parses a JSON file containing game events and sends them to the subscribed channel.
```bash
report data/events1.json
```

**4. Leave a channel (Unsubscribe):**
```bash
exit <channel_name>
# Example: exit Germany_Japan
```

**5. Logout and Disconnect:**
Safely disconnects from the server, waiting for a server receipt before closing the socket.
```bash
logout
```

---

## 📡 STOMP Protocol Implementation

The system fully implements a subset of the STOMP protocol for internal client-server communication. The underlying frames exchanged over TCP include:

### Client-to-Server Frames:
* **CONNECT**: Initiates a session with the server (includes login and passcode headers).
* **SUBSCRIBE**: Registers the client to a specific topic to receive messages.
* **UNSUBSCRIBE**: Removes the client from a topic.
* **SEND**: Publishes a message to a specific topic.
* **DISCONNECT**: Safely terminates the session, requiring a receipt-id for graceful shutdown.

### Server-to-Client Frames:
* **CONNECTED**: Acknowledges a successful login.
* **MESSAGE**: Broadcasts a published message to all clients subscribed to the relevant topic.
* **RECEIPT**: Confirms that a client's request (e.g., Subscribe, Disconnect) was successfully processed.
* **ERROR**: Sent when a protocol violation occurs (e.g., wrong password, sending without subscription), followed by connection termination.

---

## 🧠 Design Highlights

* **Dependency Injection:** The server uses factories to inject protocol and encoder/decoder logic, keeping the generic Reactor/TPC code clean (Open/Closed Principle).
* **Actor Thread Pool:** Work is dispatched to an Actor Thread Pool, ensuring that a single client is only handled by one thread at a time, preventing message reordering while maintaining high concurrency.
* **Concurrency Handling:** `ConcurrentHashMap` and `ConcurrentLinkedQueue` are used extensively to manage active clients and subscriptions safely without unnecessary locking overhead.

---
*Developed as part of the Systems Programming course at Ben-Gurion University of the Negev.*
