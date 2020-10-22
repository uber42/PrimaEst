#
# @file BloomFilterHashCount.py
#
# @author Pavel Chursin
# @date Oct 2020
#

import matplotlib.pyplot as plt
import numpy as np
import math
from pylab import meshgrid

def memory(capacity, errorProbably):
    bitsetSize = -((capacity * np.log(errorProbably / 100)) / 0.4761)
    return (bitsetSize / capacity) * 0.1732

capacities = np.arange(11, 1000000, 19999.78)
probabilities = np.arange(1, 99, 1.96)

x, y = meshgrid(capacities, probabilities)

z = memory(x, y)

fig = plt.figure()
ax = fig.gca(projection='3d')
surf = ax.plot_surface(x, y, z, rstride=1, cstride=1,vmin=0, vmax=np.nanmax(z), 
                       linewidth=0, cmap=plt.cm.RdBu, antialiased=True)

ax.set_xlabel('Элементы')
ax.set_ylabel('Вероятность')
ax.set_zlabel('Хэш функции')
fig.colorbar(surf, shrink=1, aspect=10, fraction=.1, panchor=(1.0, 0.0))
plt.show()