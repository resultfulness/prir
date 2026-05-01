from multiprocessing.managers import BaseManager
from queue import Queue
from sys import argv

from server import ADDRESS, PORT


class QueueManager(BaseManager):
    pass


def client(url_file_path):
    QueueManager.register("input")
    QueueManager.register("output")
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    m.connect()

    input_queue: Queue = m.input()
    output_queue: Queue = m.output()

    with open(url_file_path) as url_file:
        lines = url_file.read().splitlines()
        for line in lines:
            input_queue.put(line)

    result = []
    for _ in range(len(lines)):
        result.append(output_queue.get())

    print(result)


if __name__ == "__main__":
    if len(argv) < 2:
        print("Usage:", argv[0], "<url_file_path>")
    else:
        client(argv[1])
