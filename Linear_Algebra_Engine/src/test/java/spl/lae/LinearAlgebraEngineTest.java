package spl.lae;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import parser.ComputationNode;
import parser.ComputationNodeType;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

public class LinearAlgebraEngineTest {

    private LinearAlgebraEngine lae;

    @BeforeEach
    void setUp() {
        // יצירת מנוע עם 2 תהליכונים לטובת הבדיקות
        lae = new LinearAlgebraEngine(2);
    }

    @AfterEach
    void tearDown() throws InterruptedException {
        if (lae != null) {
            lae.shutdown();
        }
    }

    // --- Helper Methods ---

    private ComputationNode createLeaf(double[][] data) {
        // משתמש בבנאי הקיים למטריצות
        return new ComputationNode(data);
    }

    private ComputationNode createOp(ComputationNodeType type, ComputationNode... children) {
        // תיקון: המרה לרשימה ושימוש בבנאי המתאים של ComputationNode
        List<ComputationNode> childList = new ArrayList<>(Arrays.asList(children));
        return new ComputationNode(type, childList);
    }

    // --- Standard Tests ---

    @Test
    public void testSimpleAddition() {
        ComputationNode a = createLeaf(new double[][]{{1.0, 2.0}});
        ComputationNode b = createLeaf(new double[][]{{3.0, 4.0}});
        ComputationNode addNode = createOp(ComputationNodeType.ADD, a, b);

        ComputationNode result = lae.run(addNode);

        assertNotNull(result.getMatrix(), "Result matrix should not be null");
        assertEquals(4.0, result.getMatrix()[0][0], 0.001);
        assertEquals(6.0, result.getMatrix()[0][1], 0.001);
    }

    @Test
    public void testSimpleMultiplication() {
        ComputationNode a = createLeaf(new double[][]{{1.0, 2.0}});
        ComputationNode b = createLeaf(new double[][]{{3.0}, {4.0}});
        ComputationNode mulNode = createOp(ComputationNodeType.MULTIPLY, a, b);

        ComputationNode result = lae.run(mulNode);

        assertNotNull(result.getMatrix());
        assertEquals(11.0, result.getMatrix()[0][0], 0.001);
    }

    @Test
    public void testTranspose() {
        ComputationNode a = createLeaf(new double[][]{{1.0, 2.0}, {3.0, 4.0}});
        ComputationNode tNode = createOp(ComputationNodeType.TRANSPOSE, a);

        ComputationNode result = lae.run(tNode);
        double[][] resData = result.getMatrix();

        assertEquals(1.0, resData[0][0], 0.001);
        assertEquals(3.0, resData[0][1], 0.001);
        assertEquals(2.0, resData[1][0], 0.001);
        assertEquals(4.0, resData[1][1], 0.001);
    }

    @Test
    public void testNegate() {
        ComputationNode a = createLeaf(new double[][]{{1.0, -2.0}});
        ComputationNode negNode = createOp(ComputationNodeType.NEGATE, a);

        ComputationNode result = lae.run(negNode);
        
        assertEquals(-1.0, result.getMatrix()[0][0], 0.001);
        assertEquals(2.0, result.getMatrix()[0][1], 0.001);
    }

    @Test
    public void testComplexTree() {
        // (A + B) * C
        ComputationNode a = createLeaf(new double[][]{{1.0, 1.0}});
        ComputationNode b = createLeaf(new double[][]{{2.0, 2.0}});
        ComputationNode c = createLeaf(new double[][]{{2.0}, {0.0}});

        ComputationNode sumNode = createOp(ComputationNodeType.ADD, a, b);
        ComputationNode rootNode = createOp(ComputationNodeType.MULTIPLY, sumNode, c);

        ComputationNode result = lae.run(rootNode);
        
        assertEquals(6.0, result.getMatrix()[0][0], 0.001);
    }

    // --- Edge Case / Error Handling Tests ---

    @Test
    public void testAdditionDimensionMismatch() {
        ComputationNode a = createLeaf(new double[][]{{1.0, 2.0}});
        ComputationNode b = createLeaf(new double[][]{{1.0, 2.0, 3.0}});
        
        ComputationNode addNode = createOp(ComputationNodeType.ADD, a, b);

        assertThrows(Exception.class, () -> {
            lae.run(addNode);
        }, "Should throw exception when adding matrices of different dimensions");
    }

    @Test
    public void testMultiplicationDimensionMismatch() {
        ComputationNode a = createLeaf(new double[][]{{1.0, 2.0}}); // 1x2
        ComputationNode b = createLeaf(new double[][]{{1.0}, {2.0}, {3.0}}); // 3x1 (לא מתאים)
        
        ComputationNode mulNode = createOp(ComputationNodeType.MULTIPLY, a, b);

        assertThrows(Exception.class, () -> {
            lae.run(mulNode);
        }, "Should throw exception when multiplying matrices with incompatible dimensions");
    }

    // --- Advanced Edge Cases ---

    @Test
    public void testUnaryOperatorWithTooManyOperands() {
        ComputationNode a = createLeaf(new double[][]{{1.0}});
        ComputationNode b = createLeaf(new double[][]{{2.0}});
        
        // NEGATE אמור לקבל רק ילד אחד
        ComputationNode invalidNode = createOp(ComputationNodeType.NEGATE, a, b);

        assertThrows(Exception.class, () -> {
            lae.run(invalidNode);
        }, "Should throw exception when Unary operator has more than 1 operand");
    }

    @Test
    public void testBinaryOperatorWithInsufficientOperands() {
        ComputationNode a = createLeaf(new double[][]{{1.0}});
        
        // ADD חייב לקבל לפחות שני ילדים
        ComputationNode invalidNode = createOp(ComputationNodeType.ADD, a);

        assertThrows(Exception.class, () -> {
            lae.run(invalidNode);
        }, "Should throw exception when Binary operator has less than 2 operands");
    }

    @Test
    public void testAssociativityLogic() {
        // בדיקת שרשור: ADD(A, B, C)
        ComputationNode a = createLeaf(new double[][]{{1.0}});
        ComputationNode b = createLeaf(new double[][]{{2.0}});
        ComputationNode c = createLeaf(new double[][]{{3.0}});
        
        ComputationNode multiAddNode = createOp(ComputationNodeType.ADD, a, b, c);

        // חובה להפעיל את הפונקציה שמסדרת את העץ לבינארי לפני הריצה במנוע
        multiAddNode.associativeNesting();

        ComputationNode result = lae.run(multiAddNode);
        
        assertEquals(6.0, result.getMatrix()[0][0], 0.001);
    }
}