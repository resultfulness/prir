from queue import Queue
from manager import QueueManager, PORT, ADDRESS


def main():
    QueueManager.register("input")
    QueueManager.register("output")
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    m.connect()

    input_queue: Queue = m.input()
    output_queue: Queue = m.output()

    i = 0
    while True:
        row, vector = input_queue.get()
        print("New row :D", i)
        i += 1
        output_queue.put(sum([r * v[0] for r, v in zip(row, vector)]))


if __name__ == "__main__":
    main()
