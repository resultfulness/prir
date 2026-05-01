import asyncio
import threading
from multiprocessing.managers import BaseManager
from queue import Queue

PORT = 50002
ADDRESS = "127.0.0.1"


class QueueManager(BaseManager):
    pass


async def server():
    input_queue = Queue()
    output_queue = Queue()
    QueueManager.register("input", callable=lambda: input_queue)
    QueueManager.register("output", callable=lambda: output_queue)
    m = QueueManager(address=(ADDRESS, PORT), authkey=b"abracadabra")
    s = m.get_server()
    threading.Thread(target=s.serve_forever).start()


if __name__ == "__main__":
    asyncio.run(server())
