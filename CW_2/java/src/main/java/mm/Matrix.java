package mm;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class Matrix {
    public static Matrix fromFile(
        String filename
    ) throws IOException, MatrixFromFileException {
        try {
            BufferedReader reader = new BufferedReader(
                new FileReader(filename)
            );

            int rows = Integer.parseInt(reader.readLine());
            int columns = Integer.parseInt(reader.readLine());
            Matrix matrix = new Matrix(rows, columns);

            String line;
            for (int i = 0; i < rows; i++) {
                line = reader.readLine();
                String[] snums = line.split("\s+");
                for (int j = 0; j < columns; j++) {
                    // last num has dot at the end, remove
                    if (i == rows - 1 && j == columns - 1) {
                        snums[j] = snums[j].substring(0, snums[j].length() - 1);
                    }
                    matrix.values[i][j] = Double.parseDouble(snums[j]);
                }
            }

            reader.close();
            return matrix;
        } catch (FileNotFoundException e) {
            throw new MatrixFromFileException(
                "file '" + filename + "' not found"
            );
        } catch (NumberFormatException e) {
            throw new MatrixFromFileException(
                "file '" + filename + "' has invalid format"
            );
        }
    }
    public int rows;
    public int columns;

    public double[][] values;

    public Matrix(int rows, int columns) {
        this.rows = rows;
        this.columns = columns;
        this.values = new double[rows][columns];
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append("[");
        for (int i = 0; i < this.rows; i++) {
            for (int j = 0; j < this.columns; j++) {
                sb.append("\t");
                sb.append(String.format("%.6f", this.values[i][j]));
            }
            sb.append(i == this.rows - 1 ? "\t]" : "\n");
        }

        return sb.toString();
    }
}

class MatrixFromFileException extends Exception {
    public MatrixFromFileException(String s) {
        super(s);
    }
}
