package spl.lae;

import parser.*;
import memory.*;
import scheduling.*;

import java.util.ArrayList;
import java.util.List;

public class LinearAlgebraEngine {

    private SharedMatrix leftMatrix = new SharedMatrix();
    private SharedMatrix rightMatrix = new SharedMatrix();
    private TiredExecutor executor;

    public LinearAlgebraEngine(int numThreads) {
        // TODO: create executor with given thread count
        this.executor = new TiredExecutor(numThreads);
    }

    public ComputationNode run(ComputationNode computationRoot) {
        // TODO: resolve computation tree step by step until final matrix is produced
        while (computationRoot.getNodeType() != ComputationNodeType.MATRIX) {
            ComputationNode nextNode = computationRoot.findResolvable();

            if (nextNode == null) {
                break;
            }

            loadAndCompute(nextNode);
        }
        return computationRoot;
    }

    public void loadAndCompute(ComputationNode node) {
        // TODO: load operand matrices
        // TODO: create compute tasks & submit tasks to executor
        // הוסף את זה בתוך loadAndCompute, לפני הטעינה של המטריצות או יצירת המשימות
List<ComputationNode> children = node.getChildren();
ComputationNodeType type = node.getNodeType();

// בדיקה לאופרטורים בינאריים (חייבים 2 ילדים)
if ((type == ComputationNodeType.ADD || type == ComputationNodeType.MULTIPLY) && children.size() != 2) {
    throw new IllegalArgumentException("Binary operator " + type + " requires exactly 2 operands, got " + children.size());
}

// בדיקה לאופרטורים אונאריים (חייבים ילד 1)
if ((type == ComputationNodeType.NEGATE || type == ComputationNodeType.TRANSPOSE) && children.size() != 1) {
    throw new IllegalArgumentException("Unary operator " + type + " requires exactly 1 operand, got " + children.size());
}
  
        List<Runnable> tasks = null;
        if(type == ComputationNodeType.NEGATE || type == ComputationNodeType.TRANSPOSE) {
           leftMatrix.loadRowMajor(children.get(0).getMatrix());
        } else if(type == ComputationNodeType.ADD || type == ComputationNodeType.MULTIPLY) {
            leftMatrix.loadRowMajor(children.get(0).getMatrix());
            rightMatrix.loadRowMajor(children.get(1).getMatrix());
        }
        
        switch(type) {
            case ADD:
                tasks = createAddTasks();
                break;
            case MULTIPLY:
                rightMatrix.loadColumnMajor(children.get(1).getMatrix());
                tasks = createMultiplyTasks();
                break;
            case NEGATE:
                tasks = createNegateTasks();
                break;
            case TRANSPOSE:
                tasks = createTransposeTasks();
                break;
            default:
                throw new IllegalArgumentException("Unsupported computation node type: " + type);
        }
        executor.submitAll(tasks);
        double[][] resultData = leftMatrix.readRowMajor();
        node.resolve(resultData);

    }

    public List<Runnable> createAddTasks() {
        // TODO: return tasks that perform row-wise addition
        // ודא שהמטריצות נטענו
        if (leftMatrix == null || rightMatrix == null) return new ArrayList<>();

        // בדיקת מימדים לחיבור: חייבים להיות זהים בשורות ובעמודות
        int rows = leftMatrix.length();
        int cols = leftMatrix.get(0).length();

        if (rightMatrix.length() != rows || rightMatrix.get(0).length() != cols) {
         throw new IllegalArgumentException("Dimension mismatch for ADD: " 
        + rows + "x" + cols + " vs " + rightMatrix.length() + "x" + rightMatrix.get(0).length());
        }
        List<Runnable> tasks = new java.util.ArrayList<>();
         int numRows = leftMatrix.length();

         for (int i = 0; i < numRows; i++) {
             final int rowIndex = i;
             tasks.add(() -> {
                SharedVector v1 = leftMatrix.get(rowIndex);
                SharedVector v2 = rightMatrix.get(rowIndex);
                v1.add(v2);
             });
         }
         return tasks;
    }

    public List<Runnable> createMultiplyTasks() {
        // TODO: return tasks that perform row × matrix multiplication
        // ודא שהמטריצות נטענו
        if (leftMatrix == null || rightMatrix == null) return new ArrayList<>();

// בדיקת מימדים לכפל: עמודות של שמאל == שורות של ימין
// הערה: אם rightMatrix נטענה כ-Column Major, אז rightMatrix.length() מייצג את מספר העמודות המקוריות,
// ו-rightMatrix.get(0).length() מייצג את מספר השורות המקוריות.
// אם rightMatrix נטענה רגיל (Row Major), אז length() זה השורות.

// הנחה: בקוד שלך טענת את rightMatrix כ-Column Major לכפל יעיל?
// אם כן: מספר השורות של המטריצה הימנית הוא rightMatrix.get(0).length()
// אם לא (טענת כ-Row Major): מספר השורות הוא rightMatrix.length()

// נניח המימוש הנפוץ (ימין נטען כ-Column Major):
        int leftCols = leftMatrix.get(0).length();
         int rightRows = rightMatrix.get(0).length(); // כי ב-Column Major כל וקטור הוא עמודה, אז האורך שלו הוא מספר השורות

        if (leftCols != rightRows) {
           throw new IllegalArgumentException("Dimension mismatch for MULTIPLY: Left cols (" + leftCols + ") != Right rows (" + rightRows + ")");
        }
        List<Runnable> tasks = new java.util.ArrayList<>();
        int numRows = leftMatrix.length();

        for (int i = 0; i < numRows; i++) {
            final int rowIndex = i;
            tasks.add(() -> {
                SharedVector v1 = leftMatrix.get(rowIndex);
                v1.vecMatMul(rightMatrix);
            });
        }
        return tasks;
    }

    public List<Runnable> createNegateTasks() {
        // TODO: return tasks that negate rows
        List<Runnable> tasks = new java.util.ArrayList<>();
        int numRows = leftMatrix.length();

        for (int i = 0; i < numRows; i++) {
            final int rowIndex = i;
            tasks.add(() -> {
                SharedVector v1 = leftMatrix.get(rowIndex);
                v1.negate();
            });
        }
        return tasks;
    }

    public List<Runnable> createTransposeTasks() {
        // TODO: return tasks that transpose rows
        List<Runnable> tasks = new java.util.ArrayList<>();
        int numRows = leftMatrix.length();

        for (int i = 0; i < numRows; i++) {
            final int rowIndex = i;
            tasks.add(() -> {
                SharedVector v1 = leftMatrix.get(rowIndex);
                v1.transpose();
            });
        }
        return tasks;
    }

    public String getWorkerReport() {
        // TODO: return summary of worker activity
        return executor.getWorkerReport();
    }

    public void shutdown() throws InterruptedException {
        executor.shutdown();
    }
}
