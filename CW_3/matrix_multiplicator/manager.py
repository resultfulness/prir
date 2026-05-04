from sys import argv
from multiprocessing.managers import BaseManager
from queue import Queue
import subprocess

PORT = 50001
ADDRESS = "127.0.0.1"


class QueueManager(BaseManager):
    pass


def main():
    input_queue = Queue()
    output_queue = Queue()
    QueueManager.register("input", callable=lambda: input_queue)
    QueueManager.register("output", callable=lambda: output_queue)
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    s = m.get_server()

    workers: list[subprocess.Popen] = []
    if len(argv) > 1:
        workers_num = int(argv[1])
        print(f"Starting {workers_num} workers locally")
        for _ in range(workers_num):
            worker = subprocess.Popen(
                ["python", "worker.py"],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )
            workers.append(worker)

    print("Starting server")
    try:
        s.serve_forever()
    except KeyboardInterrupt:
        for worker in workers:
            worker.terminate()


if __name__ == "__main__":
    main()
