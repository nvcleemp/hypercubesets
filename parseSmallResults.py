import sys

if len(sys.argv) < 2: exit()

rank = int(sys.argv[1])

allVertices = set()
for i in range(1 << rank):
    allVertices.add((rank*'0' + (bin(i)[2:]))[-rank:])

def printComplement(l):
    ls = set(l)
    complement = list(allVertices - ls)
    complement.sort()
    print '{(%s)}' % ('),('.join(complement))

isosets = set()

for l in open('isosmallsets%d' % rank):
    isosets.add(int(l.split()[1]))

for l in open('smallsets%d' % rank):
    nr, s = l.strip().split(':')
    nr = int(nr)
    if nr in isosets:
        l = s.strip().split()
        if l:
           print '{(%s)}' % ('),('.join(l))
        else:
           print '{}'
        if len(l)< (1<<(rank-1)):
            printComplement(l)
