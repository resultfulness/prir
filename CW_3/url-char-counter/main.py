import asyncio
import os
import threading
from sys import argv

from client import client
from server import server
from worker import worker


async def main():
    if len(argv) < 3:
        print("Usage:", argv[0], "<url_file_path> <worker_count>")
        return

    url_file_path = argv[1]
    if not os.path.exists(url_file_path):
        print("file '", url_file_path, "' does not exist", sep="")
        return

    try:
        worker_count = int(argv[2])
        if worker_count <= 0:
            raise ValueError
    except ValueError:
        print("worker count must be a positive integer")
        return

    await server()
    threading.Thread(target=client, args=[url_file_path]).start()
    for i in range(worker_count):
        threading.Thread(target=worker, args=[i]).start()


if __name__ == "__main__":
    asyncio.run(main())
