package parser;

import java.util.List;

public class ComputationNode {

    private ComputationNodeType nodeType;
    private List<ComputationNode> children = null;
    private double[][] matrix = null; // only used for MATRIX nodes

    public ComputationNode(String operatorStr, List<ComputationNode> children) throws IllegalArgumentException {
        this.nodeType = mapOperator(operatorStr);
        this.children = children;
    }

    private ComputationNodeType mapOperator(String operatorStr) throws IllegalArgumentException {
        switch (operatorStr) {
            case "+":
                return ComputationNodeType.ADD;
            case "*":
                return ComputationNodeType.MULTIPLY;
            case "-":
                return ComputationNodeType.NEGATE;
            case "T":
                return ComputationNodeType.TRANSPOSE;
            default:
                throw new IllegalArgumentException("Unknown operator: " + operatorStr);
        }
    }

    public ComputationNode(ComputationNodeType nodeType, List<ComputationNode> children) {
        this.nodeType = nodeType;
        this.children = children;
    }

    public ComputationNode(double[][] matrix) {
        this.nodeType = ComputationNodeType.MATRIX;
        this.matrix = matrix;
    }

    public ComputationNodeType getNodeType() {
        return nodeType;
    }

    public List<ComputationNode> getChildren() {
        return children;
    }

    /**
     * Recursively finds the first resolvable node in the tree.
     * A resolvable node is defined as a node that is not of type MATRIX,
     * with children that are all of type MATRIX.
     */
    public ComputationNode findResolvable() {
        if (nodeType == ComputationNodeType.MATRIX) {
            return null;
        }
        for (ComputationNode child : children) {
            if (child.getNodeType() != ComputationNodeType.MATRIX) {
                ComputationNode res = child.findResolvable();
                if (res != null) {
                    return res;
                }
            }
        }
        return this;
    }

    /**
     * Restructures the tree to ensure that operations with more than two operands
     * are nested in a left-associative manner.
     * For example, A + B + C becomes (A + B) + C.
     * Effectively, this converts n-ary operations (n > 2) into binary operations.
     */
    public void associativeNesting() {
        if (children != null && children.size() > 2) {
            ComputationNode lastChild = children.remove(children.size() - 1);
            ComputationNode newNode = new ComputationNode(nodeType, children);
            children = List.of(newNode, lastChild);
            newNode.associativeNesting();
        }
    }

    /**
     * Resolves this node by setting its type to MATRIX and storing the computed matrix.
     */
    public void resolve(double[][] matrix) {
        this.nodeType = ComputationNodeType.MATRIX;
        this.children = null;
        this.matrix = matrix;
    }

    public double[][] getMatrix() {
        if (matrix == null) {
            throw new IllegalStateException("This node does not contain a matrix.");
        }
        return matrix;
    }


}