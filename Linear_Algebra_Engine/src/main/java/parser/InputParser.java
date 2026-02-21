package parser;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;

import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.util.*;

public class InputParser {

    private final ObjectMapper mapper = new ObjectMapper();

    public ComputationNode parse(String inputPath) throws ParseException {
        try {
            File inputFile = new File(inputPath);
            JsonNode rootJsonNode = mapper.readTree(inputFile);
            ComputationNode root = parseJsonNode(rootJsonNode);
            return root;
        } catch (IOException e) {
            throw new ParseException("Failed to read the input JSON file: " + e.getMessage(), 0);
        }
    }

    private ComputationNode parseJsonNode(JsonNode jsonNode) throws ParseException {
        if (jsonNode.has("operator") && jsonNode.has("operands")) {
            String operatorStr = jsonNode.get("operator").asText();
            ArrayNode operandJsonNodes = (ArrayNode) jsonNode.get("operands");
            List<ComputationNode> operands = new ArrayList<>();
            for (int i = 0; i < operandJsonNodes.size(); i++) {
                operands.add(parseJsonNode(operandJsonNodes.get(i)));
            }
            return new ComputationNode(operatorStr, operands);
        }
        else if (jsonNode.isArray()) {
            if (jsonNode.size() == 0) {
                throw new ParseException("Empty array cannot be parsed as DataNode.", 0);
            }
            // Check if it's a vector (1D array)
            if (jsonNode.get(0).isNumber()) {
                throw new ParseException("Vectors (1D arrays) are not supported as standalone nodes.", 0);
            }
            // Otherwise, it's a matrix (2D array)
            else {
                double[][] matrix = new double[jsonNode.size()][];
                int width = jsonNode.get(0).size();
                for (int i = 0; i < jsonNode.size(); i++) {
                    JsonNode rowJsonNode = jsonNode.get(i);
                    if (rowJsonNode.size() != width) {
                        throw new ParseException("Inconsistent row sizes in matrix.", 0);
                    }
                    if (!rowJsonNode.isArray()) {
                        throw new ParseException("Invalid matrix row: " + rowJsonNode.toString(), 0);
                    }
                    matrix[i] = new double[rowJsonNode.size()];
                    for (int j = 0; j < rowJsonNode.size(); j++) {
                        matrix[i][j] = rowJsonNode.get(j).asDouble();
                    }
                }
                return new ComputationNode(matrix);
            }
        }
        else { throw new ParseException("Invalid node structure: " + jsonNode.toString(), 0); }
    }

}