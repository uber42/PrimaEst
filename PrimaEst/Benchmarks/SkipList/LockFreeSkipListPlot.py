#
# @file LockFreeSkipListPlot.h
#
# @author Pavel Chursin
# @date Oct 2020
#

import matplotlib.pyplot as plt
import numpy as np

from scipy.interpolate import interp1d

selection = [
	100,
	1000
]

with open("LockFreeSkipListSelection.bin", mode='rb') as file:
    fileContent = file.read()

dataset = []

for i in range(0, len(fileContent), 4):
    dataset.append(
        int.from_bytes(
            fileContent[i:i + 4],
            byteorder='little', signed=True)
        )

setselection = dataset[:len(dataset) // 2]
findselection = dataset[len(dataset) // 2:]

fig, ax = plt.subplots()
plt.xlabel("Количество элементов", fontsize=16)
plt.ylabel("Время 10e-3с", fontsize=16)
ax.plot(selection, setselection, label="Insert")
ax.plot(selection, findselection, label="Search")
ax.legend()

fig.savefig("./LockFreeSkipList")
plt.show()