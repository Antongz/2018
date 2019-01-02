import matplotlib.pyplot as plt
import csv

x = []
y = []
itr = []

i = 0
with open('data.txt','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=' ')
    for row in plots:
        if (int(row[0]) > 2000 ):
            temp = 0
        else:
            temp = int(row[0])

        x.append(temp)
        
        if (int(row[1]) > 2000):
            temp = 0
        else:
            temp = int(row[1])

        y.append(temp)
        itr.append(int(i))
        i = i + 1



#plt.plot(x,y, label='Loaded from file!')
#plt.plot(x, x, 'r')

plt.xlim(0, 10000)
plt.ylim(0, 700)

plt.plot(itr, x, 'b', linewidth=0.5, label='1')
plt.plot(itr, y, 'orange', linewidth=0.5,label='2')
#plt.plot(itr, x, 'bs')
#plt.plot(itr, y, 'ro')

plt.xlabel('sample')
plt.ylabel('time to access')
#plt.title('Interesting Graph\nCheck it out')
plt.legend()
plt.show()
