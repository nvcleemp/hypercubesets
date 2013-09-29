import sys

#write header
sys.stdout.write('>>multicode le<<')

def setToGraph(s):
    #s is given as a list
    completeGraphOrder = len(s)
    extraVertices = 0
        
    distances = []

    for i in range(completeGraphOrder):
        distances.append([0]*completeGraphOrder)

    for i in range(completeGraphOrder-1):
        for j in range(i+1,completeGraphOrder):
            distance = 0
            for a,b in zip(s[i],s[j]):
                if a!=b:
                    distance += 1
            distances[i][j] = distance
            distances[j][i] = distance
            extraVertices += distance - 1

    if completeGraphOrder == 3: extraVertices += 3

    adjList = []
    for i in range(completeGraphOrder + extraVertices):
        adjList.append([])

    extraVertex = completeGraphOrder
    for i in range(completeGraphOrder-1):
        for j in range(i+1,completeGraphOrder):
            if distances[i][j] == 1:
                adjList[i].append(j)
            else:
                adjList[i].append(extraVertex)
                for k in range(distances[i][j]-2):
                    adjList[extraVertex].append(extraVertex+1)
                    extraVertex += 1
                adjList[j].append(extraVertex)
                extraVertex+=1
    if completeGraphOrder == 3:
        for i in range(completeGraphOrder):
            adjList[i].append(extraVertex)
            extraVertex += 1

    if completeGraphOrder + extraVertices > 252: 
        sys.stderr.write('Too big')
        exit()

    code = '%c' % (completeGraphOrder + extraVertices)
    
    for i in range(completeGraphOrder + extraVertices-1):
        for neighbour in sorted(adjList[i]):
            code += '%c' % (neighbour+1)
        code += '%c' % 0
    
    sys.stdout.write(code) 

#l1 = ['0001','0010','0100','1000']       
#l2 = ['1110','0010','0100','1000']       
#
#setToGraph(l1)
#setToGraph(l2)
counter = 0

for l in sys.stdin:
    counter += 1
    #if counter != 889: continue
    l = l.strip().strip('}').strip('{')
    if len(l):
        #s = l.split(',')
        s = l.split()
        setToGraph(s)
