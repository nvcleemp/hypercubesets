rank = int(raw_input('rank? '))


isosets = set()

for l in open('isosets%d' % rank):
    isosets.add(int(l.split()[1]))

for l in open('sets%d' % rank):
    nr, s = l.strip().split(':')
    nr = int(nr)
    if nr in isosets:
        l = s.split()
        if l:
           print '{(%s)}' % ('),('.join(l))
        else:
           print '{}'
