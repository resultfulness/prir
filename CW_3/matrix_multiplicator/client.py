from queue import Queue
from manager import QueueManager, PORT, ADDRESS
import itertools as it
from sys import argv

BATCH_SIZE = 16


def read_matrix(f: str):
    matrix = []
    with open(f) as file:
        rows = int(file.readline())
        columns = int(file.readline())
        for _ in range(rows):
            tmp = []
            for _ in range(columns):
                tmp.append(float(file.readline()))
            matrix.append(tmp)

    return matrix


def main():
    matrix = read_matrix("A.dat")
    vector_raw = read_matrix("X.dat")
    vector = list(it.chain.from_iterable(vector_raw))
    print("Load finished")

    QueueManager.register("input")
    QueueManager.register("output")
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    m.connect()

    input_queue: Queue = m.input()
    output_queue: Queue = m.output()

    print("Batch distribution started")
    for i in range(0, len(matrix), BATCH_SIZE):
        rows = matrix[i : i + BATCH_SIZE]
        input_queue.put((i, rows, vector), False)

    print("Collecting results")
    results = []
    while True:
        batch = output_queue.get()
        results.append(batch)
        if batch[0] + len(batch[1]) >= len(matrix):
            break

    print("Processing results")
    results.sort(key=lambda r: r[0])
    result = list(it.chain.from_iterable(map(lambda r: r[1], results)))

    if len(argv) > 1:
        with open(argv[1], "w") as output_file:
            output_file.write(f"{len(matrix)}\n")
            output_file.write("1\n")
            output_file.writelines(map(lambda v: f"{v}\n", result))
    else:
        print(result)


if __name__ == "__main__":
    main()
