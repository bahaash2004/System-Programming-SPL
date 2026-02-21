package memory;

import java.util.concurrent.locks.ReadWriteLock;

public class SharedVector {

    private double[] vector;
    private VectorOrientation orientation;
    private ReadWriteLock lock = new java.util.concurrent.locks.ReentrantReadWriteLock();

    public SharedVector(double[] vector, VectorOrientation orientation) {
        this.vector=vector;
        this.orientation=orientation;
    }

    public double get(int index) {
        // TODO: return element at index (read-locked)
        readLock();

        try{
            if(index<0 || index >= vector.length){
                throw new IndexOutOfBoundsException("Index " + index + " out of bounds for vector length " + vector.length);
            }
            return vector[index];
        }
        finally{
            readUnlock();
        }
    }

    public int length() {
        // TODO: return vector length
        readLock();
        try{
            return vector.length;
        }
        finally{
            readUnlock();
        }
    }

    public VectorOrientation getOrientation() {
        // TODO: return vector orientation
        readLock();
        try{
            return this.orientation;
        }
        finally{
            readUnlock();
        }
    }

    public void writeLock() {
        lock.writeLock().lock();
    }

    public void writeUnlock() {
        lock.writeLock().unlock();

    }

    public void readLock() {
        lock.readLock().lock();
    }

    public void readUnlock() {
        lock.readLock().unlock();
    }

    public void transpose() {
        // TODO: transpose vector
        writeLock();
        try{
            if(this.orientation == VectorOrientation.ROW_MAJOR){
                this.orientation=VectorOrientation.COLUMN_MAJOR;
            }
            else{
                this.orientation = VectorOrientation.ROW_MAJOR;
            }
        }
        finally{
            writeUnlock();
        }
    }


    public void add(SharedVector other) {
        // TODO: add two vectors
        if (this.length() != other.length()) {
        throw new IllegalArgumentException("Vector length mismatch: " + this.length() + " != " + other.length());
    }
        writeLock();
        other.readLock();
        try{
            for(int i =0 ;i<this.vector.length ;i++){
                this.vector[i] += other.get(i);
            }
        } finally{
            other.readUnlock();
            writeUnlock();
        }
    }

    public void negate() {
        // TODO: negate vector
        writeLock();
        try{
            for(int i=0 ; i<vector.length ; i++){
                vector[i]= -vector[i];
            }
        }finally{
            writeUnlock();
        }
    }

    public double dot(SharedVector other) {
        // TODO: compute dot product (row · column)
        if (this.length() != other.length()) {
        throw new IllegalArgumentException("Dot product length mismatch: " + this.length() + " != " + other.length());
    }
        readLock();
        other.readLock();
        try{
            double sum =0 ;
            for(int i=0 ; i<vector.length ; i++){
                sum += this.vector[i] * other.get(i);
            }
            return sum;
        }finally{
            other.readUnlock();
            readUnlock();

        }
    }

    public void vecMatMul(SharedMatrix matrix) {
        // אנחנו עומדים לשנות את הוקטור של עצמנו, אז חייבים לנעול לכתיבה
        writeLock();
        try {
            double[] result;
            int matrixLen = matrix.length(); // מספר הוקטורים במטריצה

            // מקרה 1: המטריצה בנויה מעמודות (Column Major)
            // זה המקרה הקל והיעיל: כל וקטור במטריצה הוא עמודה.
            // האיבר ה-j בתוצאה הוא פשוט המכפלה הסקלרית (dot) של הוקטור שלנו עם העמודה ה-j.
            if (matrix.getOrientation() == VectorOrientation.COLUMN_MAJOR) {
                int cols = matrixLen;
                result = new double[cols];

                // בדיקת תקינות מימדים בסיסית
                if (cols > 0 && this.vector.length != matrix.get(0).length()) {
                     throw new IllegalArgumentException("Dimension mismatch: Vector length must match Matrix rows");
                }

                for (int j = 0; j < cols; j++) {
                    SharedVector columnVec = matrix.get(j);
                    // שימוש בפונקציית ה-dot שכבר מימשנו (היא נועלת לקריאה בעצמה)
                    result[j] = this.dot(columnVec);
                }
            } 
           // מקרה 2: המטריצה בנויה משורות (Row Major)  
            else {
                if (matrixLen == 0) return;
                
                int rows = matrixLen;
                int cols = matrix.get(0).length();
                
                if (this.vector.length != rows) {
                    throw new IllegalArgumentException("Dimension mismatch...");
                }

                result = new double[cols];

                for (int i = 0; i < rows; i++) {
                    double scalar = this.vector[i]; 
                    SharedVector rowVec = matrix.get(i);
                    
                    // --- השיפור הגדול: שליפה חד-פעמית ---
                    // במקום לגשת לתאים בודדים ולנעול כל פעם,
                    // אנחנו מעתיקים את השורה למערך מקומי בפעולה אטומית אחת.
                    double[] rowData = rowVec.getValues(); 
                    
                    // עכשיו הלולאה הפנימית היא מתמטיקה טהורה ומהירה מאוד (בלי נעילות)
                    for (int j = 0; j < cols; j++) {
                        result[j] += scalar * rowData[j];
                    }
                }
            }

            // עדכון הוקטור הפנימי לתוצאה החדשה
            this.vector = result;
            
        } finally {
            writeUnlock();
        }
    }




    // פונקציית עזר שהוספנו כדי שהמטריצה תוכל לקבל את המידע הגולמי
    // (נדרש עבור SharedMatrix.readRowMajor)
    public double[] getValues(){
        readLock();
        try{
            return vector.clone();
        }
        finally{
            readUnlock();
        }
    }
}
