package memory;
/**
 * SharedVectors and SharedMatrices are oriented in either row-major or column-major order.
 * In a SharedVector, this determines whether the vector is treated as a row or a column.
 * In a SharedMatrix, this determines whether the matrix is stored row-by-row or column-by-column.
 */
public enum VectorOrientation {
    ROW_MAJOR,
    COLUMN_MAJOR
}
