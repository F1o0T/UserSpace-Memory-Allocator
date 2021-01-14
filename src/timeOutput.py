from pylab import *
import numpy as np
import matplotlib.pyplot as plt

def readCSV(filename):
    """
    Parses a csv file, with lines like:
    <input>,<result>[,<result>]*
    """
    res = loadtxt(filename, delimiter=',')
    resTransposed = res.transpose()
    inputs = resTransposed[0]
    results = np.delete(res, 0, axis=1)
    results = results.transpose()
    return inputs, results

def plotPoints(x, y, style='bo'):
    for run in range(0, len(y)):
        line, = plt.plot(x, y[run], style)
        
def plotBoxes(x, y):
    print(y)
    bplot = plt.boxplot(y, positions=x)
    
inputs, times = readCSV('../bin/values.csv')
plt.style.use('ggplot')
plotPoints(inputs, times)
#plotBoxes(inputs, times)

timeMin = times.min()
timeMax = times.max()
# plt.axis([fibInputs[0]-1, fibInputs[len(fibInputs)-1]+1, timeMin-10000, timeMax+10000])
plt.xlabel('active Chunks')
plt.ylabel('time (microseconds)')
plt.savefig("plot4.png")
plt.show()
