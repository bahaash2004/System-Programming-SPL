#!/usr/bin/env python3
"""
Python SQL Server for STOMP Assignment
Implements SQLite database handling for the Java server.
"""

import socket
import sys
import threading
import sqlite3
import os

SERVER_NAME = "STOMP_PYTHON_SQL_SERVER"
DB_FILE = "stomp_server.db"

def init_database():
    """Initialize the database tables if they don't exist."""
    try:
        with sqlite3.connect(DB_FILE) as conn:
            c = conn.cursor()
            
            # 1. Users table
            c.execute("""CREATE TABLE IF NOT EXISTS users (
                username TEXT PRIMARY KEY,
                password TEXT,
                registration_date TEXT
            )""")
            
            # 2. Login history table
            c.execute("""CREATE TABLE IF NOT EXISTS login_history (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT,
                login_time TEXT,
                logout_time TEXT
            )""")
            
            # 3. File tracking table
            c.execute("""CREATE TABLE IF NOT EXISTS file_tracking (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT,
                filename TEXT,
                upload_time TEXT,
                game_channel TEXT
            )""")
            
            conn.commit()
            print(f"[{SERVER_NAME}] Database initialized successfully.")
    except Exception as e:
        print(f"[{SERVER_NAME}] Error initializing database: {e}")

def execute_sql(sql_statement: str) -> str:
    """
    Executes a SQL statement. 
    Returns 'SUCCESS' for write ops, or 'SUCCESS|row1|row2...' for read ops.
    """
    try:
        # Connect to DB for each request (thread-safe approach)
        with sqlite3.connect(DB_FILE) as conn:
            c = conn.cursor()
            
            # Check if it's a SELECT query
            is_select = sql_statement.strip().upper().startswith("SELECT")
            
            c.execute(sql_statement)
            
            if is_select:
                rows = c.fetchall()
                # Format result: SUCCESS|('val1', 'val2')|('val3', 'val4')
                # Java expects string representation of tuples for parsing
                result_parts = ["SUCCESS"]
                for row in rows:
                    result_parts.append(str(row))
                return "|".join(result_parts)
            else:
                conn.commit()
                return "SUCCESS"
                
    except sqlite3.Error as e:
        return f"ERROR: {str(e)}"
    except Exception as e:
        return f"ERROR: {str(e)}"

def recv_null_terminated(sock: socket.socket) -> str:
    """Receives data until a null character is found."""
    data = b""
    while True:
        try:
            chunk = sock.recv(1024)
            if not chunk:
                return ""
            data += chunk
            if b"\0" in data:
                msg, _ = data.split(b"\0", 1)
                return msg.decode("utf-8", errors="replace")
        except Exception:
            return ""

def handle_client(client_socket: socket.socket, addr):
    # print(f"[{SERVER_NAME}] Connection from {addr}")
    
    try:
        while True:
            # 1. Receive SQL command
            message = recv_null_terminated(client_socket)
            if not message:
                break

            # print(f"[{SERVER_NAME}] Executing: {message}")

            # 2. Execute SQL
            response = execute_sql(message)
            
            # 3. Send response (null terminated)
            client_socket.sendall(response.encode("utf-8") + b"\0")

    except Exception as e:
        print(f"[{SERVER_NAME}] Error handling client {addr}: {e}")
    finally:
        try:
            client_socket.close()
        except Exception:
            pass

def start_server(host="127.0.0.1", port=7778):
    init_database()
    
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        server_socket.bind((host, port))
        server_socket.listen(5)
        print(f"[{SERVER_NAME}] Listening on {host}:{port}")

        while True:
            client_socket, addr = server_socket.accept()
            t = threading.Thread(
                target=handle_client,
                args=(client_socket, addr),
                daemon=True
            )
            t.start()

    except KeyboardInterrupt:
        print(f"\n[{SERVER_NAME}] Shutting down...")
    finally:
        try:
            server_socket.close()
        except Exception:
            pass

if __name__ == "__main__":
    port = 7778
    if len(sys.argv) > 1:
        try:
            port = int(sys.argv[1])
        except ValueError:
            pass

    start_server(port=port)