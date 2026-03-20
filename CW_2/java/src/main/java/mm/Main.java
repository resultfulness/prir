package mm;

import java.io.FileNotFoundException;
import java.io.IOException;

public class Main {
    public static void main(String args[]) throws IOException {
        CliArguments arguments;
        try {
            arguments = new CliArguments(args);
        } catch (IllegalArgumentException e) {
            System.err.println("error: " + e.getMessage());
            CliArguments.printUsage();
            System.exit(1);
            return;
        }

        try {
            Matrix A = Matrix.fromFile(arguments.getMatrix1Filename());
            Matrix B = Matrix.fromFile(arguments.getMatrix2Filename());

            ThreadedMatrixCalculator threadedMatrixCalculator =
                new ThreadedMatrixCalculator(A, B, arguments.getNThreads());

            threadedMatrixCalculator.calculate();
            threadedMatrixCalculator.printResults();
        } catch (
            IllegalArgumentException |
            FileNotFoundException e
        ) {
            System.err.println("error: " + e.getMessage());
            System.exit(1);
        }
    }
}
