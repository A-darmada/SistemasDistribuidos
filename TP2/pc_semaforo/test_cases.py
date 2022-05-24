import os

threadsNumber = [("1", "1"),("1", "2"),("1", "4"),("1", "8"),("2", "1"),("4", "1"),("8", "1")]
memoryPositions = ["1 ","2 ","4 ","16 ","32 "]
for j in range(10):
    for memory in memoryPositions:
        for threads in threadsNumber:
            command = "./main " + memory + " ".join(threads)
            os.system(command)
