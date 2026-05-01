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

    url_counts = []
    for _ in range(len(lines)):
        url_counts.append(output_queue.get())

    for i, url_count in enumerate(url_counts):
        print(lines[i] + ":", sep="")
        print(url_count)

    total = {}
    for url_count in url_counts:
        for char, count in url_count.items():
            if char not in total:
                total[char] = 0
            total[char] += count

    print("total:")
    print(total)


if __name__ == "__main__":
    if len(argv) < 2:
        print("Usage:", argv[0], "<url_file_path>")
    else:
        client(argv[1])
