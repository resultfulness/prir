package mm;

public class CliArguments {
    public static void printUsage() {
        System.err.println(
            "required args: <n threads> <matrix 1 filename> <matrix 2 filename>"
        );
    }

    private int nThreads;
    private String matrix1Filename;
    private String matrix2Filename;

    public CliArguments(String[] args) throws IllegalArgumentException {
        try {
            int nThreads = Integer.parseInt(args[1]);
            if (nThreads <= 0) {
                throw new IllegalArgumentException(
                    "invalid arg: n threads must be greater than 0"
                );
            }
            this.nThreads = nThreads;
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new IllegalArgumentException("missing arg: n threads");
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException(
                "invalid arg: n threads must be a number"
            );
        }

        try {
            this.matrix1Filename = args[2];
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new IllegalArgumentException(
                "missing arg: matrix 1 filename"
            );
        }

        try {
            this.matrix2Filename = args[3];
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new IllegalArgumentException(
                "missing arg: matrix 2 filename"
            );
        }
    }

    public int getNThreads() {
        return this.nThreads;
    }

    public String getMatrix1Filename() {
        return this.matrix1Filename;
    }

    public String getMatrix2Filename() {
        return this.matrix2Filename;
    }
}
