package mm;

public class CliArguments {
    public static void printUsage() {
        System.err.println(
            "required args: <n threads> <matrix 1 filename> <matrix 2 filename>"
        );
    }

    public int nThreads;
    public String matrix1Filename;

    public String matrix2Filename;

    public CliArguments(String[] args) throws CliArgumentsException {
        try {
            this.nThreads = Integer.parseInt(args[1]);
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new CliArgumentsException("missing arg: n threads");
        } catch (NumberFormatException e) {
            throw new CliArgumentsException(
                "invalid arg: n threads must be a number"
            );
        }

        try {
            this.matrix1Filename = args[2];
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new CliArgumentsException("missing arg: matrix 1 filename");
        }

        try {
            this.matrix2Filename = args[3];
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new CliArgumentsException("missing arg: matrix 2 filename");
        }
    }
}

class CliArgumentsException extends Exception {
    public CliArgumentsException(String s) {
        super(s);
    }
}
