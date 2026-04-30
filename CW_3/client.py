from queue import Queue
from manager import QueueManager, PORT, ADDRESS

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
    vector = read_matrix("X.dat")

    QueueManager.register("input")
    QueueManager.register("output")
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    m.connect()

    input_queue: Queue = m.input()
    output_queue: Queue = m.output()

    for row in matrix:
        input_queue.put((row, vector))

    result = []
    for _ in range(len(matrix)):
        result.append(output_queue.get())

    print(result)
if __name__ == "__main__":
    main()
