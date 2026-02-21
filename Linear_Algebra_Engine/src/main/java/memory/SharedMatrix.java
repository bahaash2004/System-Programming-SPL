package memory;

public class SharedMatrix {

    private volatile SharedVector[] vectors = {}; // underlying vectors
    private VectorOrientation orientation;

    public SharedMatrix() {
        // TODO: initialize empty matrix
        this.vectors = new SharedVector[0];
        this.orientation = VectorOrientation.ROW_MAJOR;
    }

    public SharedMatrix(double[][] matrix) {
        // TODO: construct matrix as row-major SharedVectors
        loadRowMajor(matrix);
    }

    public void loadRowMajor(double[][] matrix) {
        // TODO: replace internal data with new row-major matrix

        if(matrix==null || matrix.length==0){
            this.vectors = new SharedVector[0];
            return;
        }

        this.orientation = VectorOrientation.ROW_MAJOR;
        this.vectors = new SharedVector[matrix.length];

        for(int i=0 ;i<matrix.length ;i++){
            // יצירת וקטורים כאשר כל וקטור מייצג שורה
            this.vectors[i] = new SharedVector(matrix[i], VectorOrientation.ROW_MAJOR);
        }
    }

    public void loadColumnMajor(double[][] matrix) {
        // TODO: replace internal data with new column-major matrix

        if(matrix==null || matrix.length==0){
            this.vectors = new SharedVector[0];
            return;
        }

        this.orientation = VectorOrientation.COLUMN_MAJOR;
        int rows = matrix.length;
        int cols = matrix[0].length;

        this.vectors = new SharedVector[cols];

        // טרנספוזיציה: הופכים עמודות לשורות בזיכרון
        // כדי שכל SharedVector יחזיק עמודה אחת שלמה

        for(int j=0; j<cols ;j++){
            double[] columnData = new double[rows];
            for(int i=0; i< rows ;i++){
                columnData[i]= matrix[i][j];
            }
            this.vectors[j] = new SharedVector(columnData, VectorOrientation.COLUMN_MAJOR);
        }
    }


    // בתוך SharedMatrix.java
public double[][] readRowMajor() {
    if (vectors.length == 0) {
        return new double[0][0];
    }

    acquireAllVectorReadLocks(vectors);
    try {
        int rows;
        int cols;
        double[][] result;

        // >>> השינוי כאן: בדיקת האוריינטציה של הוקטור הראשון <<<
        // אם יש וקטורים, נבדוק את הסטטוס של הראשון שבהם.
        // זה קריטי כי ה-Executor אולי שינה אותם מבלי לעדכן את המטריצה.
        boolean isRowMajor = (vectors[0].getOrientation() == VectorOrientation.ROW_MAJOR);

        if (isRowMajor) { 
            // המקרה הרגיל
            rows = vectors.length;
            result = new double[rows][];
            for (int i = 0; i < rows; i++) {
                result[i] = vectors[i].getValues();
            }
        } else {
            // המקרה של Transpose (הוקטורים מסומנים כעמודות)
            // הלוגיקה כאן הופכת את הנתונים פיזית כדי להחזיר מערך דו-ממדי תקין
            cols = vectors.length;
            rows = vectors[0].length(); // אורך הוקטור הוא מספר השורות המקורי
            result = new double[rows][cols];

            for (int j = 0; j < cols; j++) {
                double[] colData = vectors[j].getValues();
                for (int i = 0; i < rows; i++) {
                    result[i][j] = colData[i];
                }
            }
        }
        return result;

    } finally {
        releaseAllVectorReadLocks(vectors);
    }
}

    public SharedVector get(int index) {
        // TODO: return vector at index
        return vectors[index];
    }

    public int length() {
        // TODO: return number of stored vectors
        return vectors.length;
    }

    public VectorOrientation getOrientation() {
        // TODO: return orientation
        return this.orientation;
    }

    private void acquireAllVectorReadLocks(SharedVector[] vecs) {
        // TODO: acquire read lock for each vector
        for (SharedVector vec : vecs) {
            vec.readLock();
        }
    }

    private void releaseAllVectorReadLocks(SharedVector[] vecs) {
        // TODO: release read locks
        for (SharedVector vec : vecs) {
            vec.readUnlock();
        }
    }

    private void acquireAllVectorWriteLocks(SharedVector[] vecs) {
        // TODO: acquire write lock for each vector
        for (SharedVector vec : vecs) {
            vec.writeLock();
        }
    }

    private void releaseAllVectorWriteLocks(SharedVector[] vecs) {
        // TODO: release write locks
        for (SharedVector vec : vecs) {
            vec.writeUnlock();
        }
    }
}
