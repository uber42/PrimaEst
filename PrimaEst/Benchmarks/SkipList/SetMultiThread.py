import matplotlib.pyplot as plt
import numpy as np

from scipy.interpolate import interp1d

with open("SetMultiThreadSelection.bin", mode='rb') as file:
    fileContent = file.read()

dataset = []

for i in range(0, len(fileContent), 4):
    dataset.append(
        int.from_bytes(
            fileContent[i:i + 4],
            byteorder='little', signed=True)
        )

selection = list(range(1, 11))

findselection = dataset[:len(dataset) // 2]
findlockfreeselection = dataset[len(dataset) // 2:]

fig, ax = plt.subplots()
plt.xlabel("Количество потоков", fontsize=16)
plt.ylabel("Время 10e-3с", fontsize=16)
ax.plot(selection, findselection, label="Implementation 1")
ax.plot(selection, findlockfreeselection, label="Implementation 2")
ax.legend()

fig.savefig("./SetMultiThread")
plt.show()