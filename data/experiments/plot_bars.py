import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np

# amount of triangles to time
import csv
import sys


def multibarplot(ax, data, xlabels, ylabels, fill_ratio = 0.8):
    l = len(data.T)
    D = len(data)
    width = fill_ratio/D

    for i,(d,ylabel) in enumerate(zip(data,ylabels)):
        ax.bar(np.arange(l) + (i-(D-1)/2) *width,d, width=width, label=ylabel,
               zorder=3)

    ax.set_xticks(np.arange(l), xlabels)
    ax.legend()
    #ax.autoscale(tight=True)


data = []
labels = []

if len(sys.argv) > 1:
    filename = sys.argv[1]
else:
    filename = input("Filename:")
#filename = "plos_sah_buckets.csv"

groups = []
xgroups = []

with open(filename) as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='#')
    for a, row in enumerate(reader):
        if  a == 0:
            for l in row:
                labels.append(l)
        else:
            if row[0] == "names":
                for value in row[1:]:
                    groups.append(value)
            else:
                xgroups += [row[0]]
                data += [[]]
                for i, field in enumerate(row[1:]):
                    data[len(data) -1].append(float(field))
            
if len(groups) != len(data[0]) - 1:
    print("groups not the right lenght")

np_data = np.array(data)
# Plotting the data with lines in different colors
plt.figure(figsize=(8, 6))

#plt.title('Rendering time')
plt.xlabel(labels[0])
plt.ylabel(labels[1])
# plt.legend()
plt.grid(zorder=0, axis='y')

#data = np.array([np.arange(i,i+5) for i in range(3)]) + .123
#plt.figure(figsize=(10,5))
ax=plt.gca()
ax.yaxis.set_major_locator(ticker.IndexLocator(base=0.5, offset=0))
print(data)
multibarplot(ax, np_data, groups, xgroups)
plt.show()
