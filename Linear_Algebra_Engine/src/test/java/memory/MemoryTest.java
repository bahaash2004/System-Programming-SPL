package memory;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;
import java.util.Arrays;

public class MemoryTest {

    @Test
    public void testVectorBasicOperations() {
        double[] data1 = {1.0, 2.0, 3.0};
        double[] data2 = {4.0, 5.0, 6.0};
        
        SharedVector v1 = new SharedVector(data1, VectorOrientation.ROW_MAJOR);
        SharedVector v2 = new SharedVector(data2, VectorOrientation.ROW_MAJOR);

        // בדיקת Dot Product
        double dotResult = v1.dot(v2); 
        // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
        assertEquals(32.0, dotResult, 0.0001, "Dot product should be 32");

        // בדיקת חיבור (Add)
        v1.add(v2);
        // v1 צריך להיות עכשיו {5, 7, 9}
        assertArrayEquals(new double[]{5.0, 7.0, 9.0}, v1.getValues(), "Addition failed");

        // בדיקת שלילה (Negate)
        v2.negate();
        assertArrayEquals(new double[]{-4.0, -5.0, -6.0}, v2.getValues(), "Negate failed");
    }

    @Test
    public void testMatrixRowMajorLoadAndRead() {
        double[][] data = {
            {1, 2},
            {3, 4}
        };
        SharedMatrix m = new SharedMatrix(data); // Default is Row Major

        // בדיקה שהמטריצה נטענה כ-ROW_MAJOR
        assertEquals(VectorOrientation.ROW_MAJOR, m.getOrientation());
        
        // בדיקה שהקריאה חזרה עובדת
        double[][] result = m.readRowMajor();
        assertTrue(Arrays.deepEquals(data, result), "ReadRowMajor failed for Row Major matrix");
    }

    @Test
    public void testMatrixColumnMajorLoadAndRead() {
        double[][] data = {
            {1, 2, 3},
            {4, 5, 6}
        };
        
        SharedMatrix m = new SharedMatrix();
        m.loadColumnMajor(data);

        // בדיקה פנימית: ה-Orientation צריך להיות COLUMN
        assertEquals(VectorOrientation.COLUMN_MAJOR, m.getOrientation());
        
        // בדיקה פנימית: בזיכרון צריכות להיות 3 עמודות (וקטורים), כל אחד באורך 2
        assertEquals(3, m.length()); 
        assertEquals(2, m.get(0).length());

        // בדיקה שהקריאה חזרה הופכת את זה נכון למערך שורות
        double[][] result = m.readRowMajor();
        assertTrue(Arrays.deepEquals(data, result), "ReadRowMajor failed for Column Major matrix");
    }

    @Test
    public void testVecMatMul_WithColumnMajorMatrix() {
        // וקטור: [1, 2]
        SharedVector v = new SharedVector(new double[]{1, 2}, VectorOrientation.ROW_MAJOR);
        
        // מטריצה (נטענת כעמודות לטובת יעילות):
        // [3, 4]
        // [5, 6]
        double[][] matData = {
            {3, 4},
            {5, 6}
        };
        SharedMatrix m = new SharedMatrix();
        m.loadColumnMajor(matData);

        // חישוב צפוי:
        // איבר 1: [1,2] dot [3,5] = 3 + 10 = 13
        // איבר 2: [1,2] dot [4,6] = 4 + 12 = 16
        // תוצאה: [13, 16]

        v.vecMatMul(m);
        assertArrayEquals(new double[]{13.0, 16.0}, v.getValues(), 0.0001);
    }

    @Test
    public void testVecMatMul_WithRowMajorMatrix() {
        // אותו מבחן, אבל המטריצה נטענת כשורות (המסלול הפחות יעיל בקוד שלך)
        SharedVector v = new SharedVector(new double[]{1, 2}, VectorOrientation.ROW_MAJOR);
        double[][] matData = {
            {3, 4},
            {5, 6}
        };
        SharedMatrix m = new SharedMatrix();
        m.loadRowMajor(matData); // Row Major

        v.vecMatMul(m);
        assertArrayEquals(new double[]{13.0, 16.0}, v.getValues(), 0.0001);
    }
}