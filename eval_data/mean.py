import numpy as np

filename = input()

rss = []
merits = []

with open(filename, 'r') as reader:
    for line in reader:
        if 'RS' in line:
            rss.append(float(line.strip().split('RS')[1].strip()))
        elif 'Merit' in line:
            merits.append(float(line.strip().split('Merit')[1].strip()))

print('Mean RS: {}'.format(np.mean(rss)))
print('Std RS: {}'.format(np.std(rss)))
print()
print('Mean Merit: {}'.format(np.mean(merits)))
print('Std Merit: {}'.format(np.std(merits)))
