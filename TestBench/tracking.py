#-*- coding: utf-8 -*-
import csv
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
import matplotlib as mpl

x =[]
y=[]
z =[]
with open('tracking.dat', 'r') as f:
    reader = csv.reader(f, delimiter = '\t')

    for row in reader:
        print row    
        x.append(row[5])
        y.append(row[6])
        z.append(row[7])

plt.scatter(x,y,z)
plt.show()