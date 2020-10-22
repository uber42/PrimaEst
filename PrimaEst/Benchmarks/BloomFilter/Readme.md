# Характеристики для фильтра Блума

Результаты

Зависимость расхода памяти в битах в зависимости от предполагаемого количества элементов и вероятности ложноположительного срабатываения.  
<p align="center"><img src="https://latex.codecogs.com/png.latex?m=-\frac{n*ln(p)}{ln^{2}(2)}"></p> где n - предполагаемое количество элементов хранящихся в фильтре Блума, p - вероятность ложноположительного срабатывания.  

![](./BloomFilterMemory.png "")  

Зависимость количества хэш функций в зависимости от предполагаемого количества элементов и вероятности ложноположительного срабатываения. 
<p align="center"><img src="https://latex.codecogs.com/png.latex?k=\frac{m}{n}ln(2)"></p>
<p align="center"><img src="https://latex.codecogs.com/png.latex?k=-\frac{n*ln(p)*ln(2)}{n * ln^{2}(2)}"></p>
<p align="center"><img src="https://latex.codecogs.com/png.latex?k=-\frac{ln(p)}{ln(2)}"></p>
<p align="center"><img src="https://latex.codecogs.com/png.latex?k=-\log_{2}{p}"></p>
где p - вероятность ложноположительного срабатывания.      
Следорвательно количество хэш-функций зависит только от выбранной вероятности.

Так как в данной работе используется 128 битная хэш функция, ее значения бьются на 4 части по 32 бита и каждый принимается как отдельный результат, то количество хэш функций делится на 4.
<p align="center"><img src="https://latex.codecogs.com/png.latex?k=-\frac{\log_{2}{p}}{4}"></p>

![](./BloomFilterHashCount.png "")


