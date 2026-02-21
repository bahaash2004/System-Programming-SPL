package spl.lae;
import java.io.IOException;

import parser.*;

public class Main {
    public static void main(String[] args) throws IOException {
      // TODO: main
      if(args.length != 3) {
          System.out.println("Usage: java -jar lae.jar <input_file> <output_file> <num_threads>");
          return;
      }
      int numThreads = Integer.parseInt(args[0]);
      String inputFile = args[1];
      String outputFile = args[2];
      LinearAlgebraEngine lae = null;
      try {
          InputParser parser = new InputParser();
          ComputationNode rootNode = parser.parse(inputFile);

          rootNode.associativeNesting();

          lae = new LinearAlgebraEngine(numThreads);

          ComputationNode resultNode = lae.run(rootNode);
          if (resultNode==null) {
            throw new RuntimeException("Computation did not produce a result matrix.");
          }
          OutputWriter.write(resultNode.getMatrix(), outputFile);
          System.out.println(lae.getWorkerReport());


      } catch (Exception e) {

        try {
            String errorMsg = "Error during computation: " + e.getMessage();
          //  if (errorMsg == null) {
                OutputWriter.write(errorMsg, outputFile);
           // }
        } catch (IOException ioException) {
          ioException.printStackTrace();
        }
        e.printStackTrace();

      } finally {
          if (lae != null) {
              try {
                  lae.shutdown();
              } catch (InterruptedException e) {
                  e.printStackTrace();
              }
          }
      }




    }
}