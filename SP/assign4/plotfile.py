import matplotlib.pyplot as plt
import numpy as np

x, y = np.loadtxt('data.txt', delimiter=' ', unpack=True)

plt.plot(x, y, label='Loaded from data.txt')

plt.xlabel('x')
plt.ylabel('y')
plt.title('assignment 4')
plt.legend()
plt.show()
