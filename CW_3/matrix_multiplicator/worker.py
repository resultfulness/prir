from queue import Queue
from manager import QueueManager, PORT, ADDRESS


def main():
    QueueManager.register("input")
    QueueManager.register("output")
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    m.connect()

    input_queue: Queue = m.input()
    output_queue: Queue = m.output()

    while True:
        try:
            batch_index, rows, vector = input_queue.get()
        except KeyboardInterrupt:
            print("Worker terminated")
            break
        print("Processing batch", batch_index)
        result = [sum(r * v for r, v in zip(row, vector)) for row in rows]
        output_queue.put((batch_index, result))


if __name__ == "__main__":
    main()
