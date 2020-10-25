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

def hashcount(errorProbably):
    return -np.log2(errorProbably)

probabilities = np.arange(0.00001, 1, 0.00001)


plt.xlabel("Вероятность", fontsize=16)
plt.ylabel("Количество хэш функций", fontsize=16)
plt.plot(probabilities, hashcount(probabilities), label="")
plt.savefig("./BloomFilterHashCount.png")
plt.show()