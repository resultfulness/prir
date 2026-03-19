package mm;

import java.io.IOException;

public class Main {
    public static void main(String args[]) throws IOException {
        try {
            CliArguments arguments = new CliArguments(args);

            Matrix A = Matrix.fromFile(arguments.matrix1Filename);
            Matrix B = Matrix.fromFile(arguments.matrix2Filename);

            ThreadedMatrixCalculator threadedMatrixCalculator =
                new ThreadedMatrixCalculator(A, B, arguments.nThreads);

            threadedMatrixCalculator.calculate();
            threadedMatrixCalculator.printResults();
        } catch (CliArgumentsException e) {
            System.err.println("error: " + e.getMessage());
            CliArguments.printUsage();
            System.exit(1);
        } catch (MatrixFromFileException | MatrixCalculatorException e) {
            System.err.println("error: " + e.getMessage());
            System.exit(1);
        }
    }
}
