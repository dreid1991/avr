import matplotlib.pyplot as plt

dat = open('bright.dat', 'r').readlines()

vals = [int(d[:-1]) >> 8 for d in dat]


plt.plot(vals)
plt.show()
