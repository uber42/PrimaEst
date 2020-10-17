import matplotlib.pyplot as plt
import numpy as np

from scipy.interpolate import interp1d

with open("FindMultiThreadSelection.bin", mode='rb') as file:
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
ax.plot(selection, findselection, label="Lock read")
ax.plot(selection, findlockfreeselection, label="Lock-Free read")
ax.legend()

fig.savefig("./FindMultiThread")
plt.show()