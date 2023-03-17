import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv('../logger/output.csv', names=['time', 'value'])

plt.figure(figsize=[5,10])
plt.xlabel('Time')
plt.ylabel('Temp')
plt.scatter(df['time'], df['value'],label='measurements')
plt.legend(loc=2)
plt.savefig('plot.pdf')
