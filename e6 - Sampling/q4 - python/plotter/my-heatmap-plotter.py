import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import math

df = pd.read_csv('../logger/output.csv', names=['time', 'value'])

tcount = 600
t0 = df['time'][0]
t1 = df['time'][len(df['time']) - 1]

bucket_duration = (t1 - t0) / tcount

print("Start Time:", t0)
print("End Time:", t1)
print("Bucket duration:", bucket_duration)

def index(timestamp):
    # calculate the time difference between the timestamp and t0
    time_diff = (timestamp - t0)
    # calculate the bucket index
    bucket_index = int(time_diff // bucket_duration)
    # return the x-coordinate of the bucket
    return bucket_index

def west(index):
    # calculate the timestamp of the left edge of the bucket
    bucket_start = t0 + (index * bucket_duration)
    # return the timestamp of the left edge of the bucket
    return bucket_start

def east(index):
    # calculate the timestamp of the right edge of the bucket
    bucket_end = t0 + ((index + 1) * bucket_duration)
    # return the timestamp of the right edge of the bucket
    return bucket_end

def south(y):
    return math.floor(y)

def north(y):
    return math.floor(y)+1

def filename2ts(filename):
    df = pd.read_csv(filename, names=['time', 'value'])
    # extract the timestamp×value pairs as a list of tuples
    ts_values = list(zip(df['time'], df['value']))
    # return the list of timestamp×value pairs
    return ts_values

def ts2heatmap(ts_values):
    heatmap = np.zeros((60, 600))
    for ts, value in ts_values:
        i = index(ts)
        w = west(i)
        e = east(i)
        s = south(value)
        n = north(value)
        heatmap[s][i] += 1 
    return heatmap

def max(heatmap):
    return np.amax(heatmap)

def gen_colormap(value, max):
    if (value == 0):
        return "#808080"
    # interpolate between blue and red based on the value
    r = int(255 * value / max)
    b = int(255 * (1 - value / max))
    g = 0
    # convert the RGB values to hex format
    hex_value = '{:02x}{:02x}{:02x}'.format(r, g, b)
    # return the hex value as a string
    return '#' + hex_value

ts = filename2ts('../logger/output.csv')
hm = ts2heatmap(ts)
m = max(hm)

plt.figure(figsize=[16,10])
fig, ax = plt.subplots()

for i, row in enumerate(hm):
    for j, element in enumerate(row):
        y=i
        x=t0+(j*bucket_duration)
        col = gen_colormap(element, m)
        ax.add_patch(Rectangle((x, y), width=bucket_duration, height=1, color=col))

plt.xlabel('Time')
plt.ylabel('Temp')
#plt.scatter(df['time'], df['value'],label='measurements')
#plt.legend(loc=2)
# set x limits to match the Rectangle patches
plt.xlim(t0, t1)
plt.ylim(0, 60)
plt.savefig('heatmap.pdf')
