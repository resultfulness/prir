package mm;

public class ThreadedMatrixCalculator {
    private class MultiplicationThread extends Thread {
        private int index;

        public MultiplicationThread(int index) {
            this.index = index;
        }

        public void run() {
            int tmp = this.index;
            while (tmp < Out.getRows() * Out.getColumns()) {
                double res = 0;
                int r = tmp / Out.getColumns();
                int c = tmp % Out.getColumns();

                for (int i = 0; i < A.getColumns(); i++) {
                    res += A.getValueAt(r, i) * B.getValueAt(i, c);
                }

                Out.setValueAt(r, c, res);

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
    ) throws IllegalArgumentException {
        int ar = A.getRows();
        int ac = A.getColumns();
        int br = B.getRows();
        int bc = B.getColumns();
        if (ac != br) {
            throw new IllegalArgumentException(
                String.format(
                    "can't multiply matrices: dimensions mismatch! " +
                    "got %dx%d * %dx%d; %d != %d",
                    ar, ac, br, bc, ac, br
                )
            );
        }

        this.A = A;
        this.B = B;
        try {
            this.Out = new Matrix(ar, bc);
        } catch (IllegalArgumentException e) { // cant fail, A and B valid
        }
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
