package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Context: Transitioning from standard Request-Response to STOMP protocol.
 * * במטלה זו, השרת צריך לתמוך בהפצת הודעות (Pub/Sub) ולא רק לענות למי שפנה אליו.
 * לדוגמה: כשלקוח אחד מדווח על גול, השרת צריך לדחוף את ההודעה לכל שאר המנויים.
 * * המטרה של מחלקה זו:
 * לנהל את המיפוי בין "מספר זהות" (connectionId) לבין ה"צינור" (ConnectionHandler).
 * זה מאפשר לשרת לפנות ללקוחות ספציפיים או לקבוצות של לקוחות, גם בלי שהם שלחו בקשה כרגע.
 */

public class ConnectionsImpl<T> implements Connections<T> {


    // Stores connection handlers mapped by their IDs
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers = new ConcurrentHashMap<>(0);
    //creates unique IDs for each clients that connects to the server
    private final AtomicInteger idCounter = new AtomicInteger(0);

    // מפה שמחזיקה רשימת הלקוחות הרשומים לכל ערוץ
    private final ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> channels = new ConcurrentHashMap<>();    



    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = connectionHandlers.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
        return false;
    }


    @Override
    public void disconnect(int connectionId) {
        ConnectionHandler<T> handler = connectionHandlers.remove(connectionId);
        if (handler != null) {
            try {
                handler.close();
            } catch (Exception e) {}   
        }

    }

    @Override
    public void send(String channel, T msg) {
        ConcurrentLinkedQueue<Integer> subscribers = channels.get(channel);
        if (subscribers != null) {
            for (Integer connectionId : subscribers) {
                send(connectionId, msg);
            }
        }
    }

    public int addConnection(ConnectionHandler<T> handler) {
        int id = idCounter.getAndIncrement();
        connectionHandlers.put(id, handler);
        return id;
    }
    
    //-- פונקציות נוספות לניהול ערוצים ומנויים --//

    public void subscribe(String channel, int connectionId) {
        // יוצר את הערוץ אם הוא לא קיים, ומוסיף את הלקוח
        channels.computeIfAbsent(channel, k -> new ConcurrentLinkedQueue<>()).add(connectionId);
    }

    public void unsubscribe(String channel, int connectionId) {
        ConcurrentLinkedQueue<Integer> subscribers = channels.get(channel);
        if (subscribers != null) {
            subscribers.remove(connectionId);
        }
    }
}