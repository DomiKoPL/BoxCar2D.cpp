from sys import argv
import io
import matplotlib.pyplot as plt

path = argv[1]
title = argv[2]

print(path)
with open(path) as file:
    seed = file.readline()
    print(seed)


    minn = []
    maxx = []
    mean = []

    for line in file.readlines():
        l = list(map(float, line.split()))
        minn.append(min(l))
        maxx.append(max(l))
        mean.append(sum(l) / len(l))
    
    plt.plot(range(len(minn)), [0]*len(minn), color='orange')
    plt.plot(range(len(minn)), minn, label='min', color='r')
    plt.plot(range(len(mean)), mean, label='mean', color='b')
    plt.plot(range(len(maxx)), maxx, label='max', color='g')
    plt.legend()
    plt.ylabel('score')
    plt.xlabel('generation')
    plt.title(title)
    plt.savefig(title+'.png')
    plt.plot()

