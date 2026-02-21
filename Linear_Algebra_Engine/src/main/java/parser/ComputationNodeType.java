package parser;

/**
 * Computation nodes hold either an operation to be applied, or a matrix (2D array).
 * Matrices are always leaf nodes, while operation nodes have children.
 */
public enum ComputationNodeType {
    ADD,
    MULTIPLY,
    NEGATE,
    TRANSPOSE,
    MATRIX,
}