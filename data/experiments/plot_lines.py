import matplotlib.pyplot as plt

# Generate random data for three test sets (x, y) tuples

# resolution to time
# bunny = [(100, 0.20), (500, 4.96), (800, 12.68)]
# armadillo = [(100, 0.25), (500, 5.89), (800, 15.90)]
# dragon = [(100, 0.28), (500, 7.08), (800, 19.99)]

# amount of triangles to time
import csv
import sys

data = []
labels = []

if len(sys.argv) > 1:
    filename = sys.argv[1]
else:
    filename = input("Filename:")
#filename = "plos_sah_buckets.csv"

scale = []

with open(filename) as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='#')
    for a, row in enumerate(reader):
        if  a == 0:
            for l in row:
                labels.append(l)
        else:
            if row[0] == "scale":
                scale = []
                for value in row[1:]:
                    scale.append(float(value))
            else:
                data += [[row[0]]]
                for i, field in enumerate(row[1:]):
                    data[len(data) -1].append(float(field))
            
print(scale)
print(data)
if len(scale) != len(data[0]) - 1:
    print("no valid scale")
    scale = [x + 4 for x in range(len(data[0]) - 1)]

# Plotting the data with lines in different colors
plt.figure(figsize=(8, 6))

color = ['r','b','y', 'o']
for i in range(len(data)):
    plt.plot(scale, data[i][1:], marker="o", linestyle='-',
             color=color[i],
             label=data[i][0])
#plt.plot(x2, y2, marker="o", linestyle='-', color='r', label='with bvh')

#plt.title('Rendering time')
plt.xlabel(labels[0])
plt.ylabel(labels[1])
plt.legend()
plt.grid(True)

plt.show()
