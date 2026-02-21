package parser;

import java.io.File;
import java.io.IOException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;

public class OutputWriter {

    private static final ObjectMapper mapper = new ObjectMapper().enable(SerializationFeature.INDENT_OUTPUT);

    public static class ResultMatrix {
        public double[][] result;
        public ResultMatrix(double[][] result) { this.result = result; }
    }

    public static class ErrorMessage {
        public String error;
        public ErrorMessage(String error) { this.error = error; }
    }

    public OutputWriter() {}

    public static void write(double[][] matrix, String filePath) throws IOException {
        File file = new File(filePath);
        mapper.writerWithDefaultPrettyPrinter().writeValue(file, new ResultMatrix(matrix));
    }

    public static void write(String error, String filePath) throws IOException {
        File file = new File(filePath);
        mapper.writerWithDefaultPrettyPrinter().writeValue(file, new ErrorMessage(error));
    }

}
