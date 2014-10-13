import matplotlib.pyplot as plt

dat = open('bright.dat', 'r').readlines()

vals = [float(d[:-1]) for d in dat]
plt.plot(vals)
plt.show()
