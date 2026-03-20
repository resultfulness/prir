package mm;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class Matrix {
    public static Matrix fromFile(
        String filename
    ) throws IOException, FileNotFoundException, IllegalArgumentException {
        try {
            BufferedReader reader = new BufferedReader(
                new FileReader(filename)
            );

            int rows = Integer.parseInt(reader.readLine());
            int columns = Integer.parseInt(reader.readLine());
            Matrix matrix = new Matrix(rows, columns);

            String line;
            for (int i = 0; i < rows; i++) {
                if ((line = reader.readLine()) == null) {
                    reader.close();
                    throw new IllegalArgumentException(
                        "file '" + filename + "' has invalid format: " +
                        "row count does not match"
                    );
                }
                String[] snums = line.split("\s+");
                if (snums.length != columns) {
                    reader.close();
                    throw new IllegalArgumentException(
                        "file '" + filename + "' has invalid format: " +
                        "column count does not match"
                    );
                }
                for (int j = 0; j < columns; j++) {
                    // last num has dot at the end, remove
                    if (
                        i == rows - 1 &&
                        j == columns - 1 &&
                        snums[j].charAt(snums[j].length() - 1) == '.'
                    ) {
                        snums[j] = snums[j].substring(0, snums[j].length() - 1);
                    }
                    matrix.values[i][j] = Double.parseDouble(snums[j]);
                }
            }

            if (reader.readLine() != null) {
                reader.close();
                throw new IllegalArgumentException(
                    "file '" + filename + "' has invalid format: " +
                    "row count does not match"
                );
            }

            reader.close();
            return matrix;
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException(
                "file '" + filename + "' has invalid format: " +
                "could not parse number"
            );
        }
    }

    private int rows;
    private int columns;

    private double[][] values;

    public Matrix(int rows, int columns) throws IllegalArgumentException {
        if (rows <= 0) {
            throw new IllegalArgumentException(
                "matrix row count invalid: must be greater than 0, got: " + rows
            );
        }
        if (columns <= 0) {
            throw new IllegalArgumentException(
                "matrix column count invalid: must be greater than 0, got: " +
                columns
            );
        }
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

    public int getRows() {
        return this.rows;
    }

    public int getColumns() {
        return this.columns;
    }

    public double getValueAt(int row, int column) {
        return this.values[row][column];
    }

    public void setValueAt(int row, int column, double value) {
        this.values[row][column] = value;
    }
}
