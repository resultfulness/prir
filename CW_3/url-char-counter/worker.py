import collections
from multiprocessing.managers import BaseManager
from queue import Queue
from urllib.request import urlopen

from server import ADDRESS, PORT


class QueueManager(BaseManager):
    pass


def count_chars(string):
    return dict(collections.Counter(string))


def fetch_url_text(url):
    with urlopen(url) as res:
        return res.read().decode()


def worker(id):
    QueueManager.register("input")
    QueueManager.register("output")
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    m.connect()

    input_queue: Queue = m.input()
    output_queue: Queue = m.output()

    id_str = f"[#{id}]" if id is not None else ""

    while True:
        url = input_queue.get()
        print(id_str, "New url :D", url)
        url_text = fetch_url_text(url)
        output_queue.put(count_chars(url_text))


if __name__ == "__main__":
    worker(None)
