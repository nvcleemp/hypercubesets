def formatSet(s):
    l = s.split()
    return "{(%s)}" % "),(".join(l)

def dualSet(s):
    pass

import sys

d = []
d2 = {}
sets = []

for l in sys.stdin:
    l = l.strip().strip('.').split()
    copy, original = l[1], l[7]
    d.append((original,copy))
    sets.append(int(copy))
    sets.append(int(original))

count = 1
for l in open('nonisosets5.clean', 'r'):
    if count in sets:
        d2[str(count)] = l.strip()
    count+=1

for original, copy in d:
    #print len(d2[original].split()), len(d2[copy].split())
    print formatSet(d2[original])
    print formatSet(d2[copy])
    print

