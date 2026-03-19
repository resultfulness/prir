package mm;

public class ThreadedMatrixCalculator {
    private class MultiplicationThread extends Thread {
        private int index;

        public MultiplicationThread(int index) {
            this.index = index;
        }

        public void run() {
            int tmp = this.index;
            while (tmp < Out.rows * Out.columns) {
                double res = 0;
                int r = tmp / Out.columns;
                int c = tmp % Out.columns;

                for (int i = 0; i < A.columns; i++) {
                    res += A.values[r][i] * B.values[i][c];
                }

                Out.values[r][c] = res;

                ThreadedMatrixCalculator.this.updateOutFrobeniusNorm(res);

                tmp += threads.length;
            }
        }
    }

    private Matrix A;
    private Matrix B;
    private Matrix Out;
    private double OutFrobeniusNorm;

    private MultiplicationThread[] threads;

    public ThreadedMatrixCalculator(
        Matrix A,
        Matrix B,
        int nThreads
    ) throws MatrixCalculatorException {
        if (A.columns != B.rows) {
            throw new MatrixCalculatorException(
                String.format(
                    "can't multiply matrices: dimensions mismatch! " +
                    "got %dx%d * %dx%d; %d != %d",
                    A.rows, A.columns, B.rows, B.columns, A.columns, B.rows
                )
            );
        }

        this.A = A;
        this.B = B;
        this.Out = new Matrix(A.rows, B.columns);
        this.OutFrobeniusNorm = 0;

        this.threads = new MultiplicationThread[nThreads];
        for (int i = 0; i < nThreads; i++) {
            this.threads[i] = new MultiplicationThread(i);
        }
    }

    public synchronized void updateOutFrobeniusNorm(double value) {
        this.OutFrobeniusNorm += value * value;
    }

    public void calculate() {
        for (MultiplicationThread multiplicationThread : this.threads) {
            multiplicationThread.start();
        }
        for (MultiplicationThread multiplicationThread : this.threads) {
            try {
                multiplicationThread.join();
            } catch (InterruptedException e) {
            }
        }
        this.OutFrobeniusNorm = Math.sqrt(OutFrobeniusNorm);
    }

    public void printResults() {
        System.out.println("result matrix:");
        System.out.println(Out);
        System.out.print("calculated frobenius norm: ");
        System.out.println(OutFrobeniusNorm);
    }
}

class MatrixCalculatorException extends Exception {
    public MatrixCalculatorException(String s) {
        super(s);
    }
}
