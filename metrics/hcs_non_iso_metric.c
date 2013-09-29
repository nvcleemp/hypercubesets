// cc -O4 -o hcs_non_iso_metric hcs_non_iso_metric.c nautil.c nauty.c

#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include <limits.h>
#include "nauty.h"

#define leer 0
#define nil 0
#define knoten 210 

#define reg 3
#define MAXVAL knoten-1


typedef unsigned char GRAPH[knoten+1][MAXVAL];
typedef unsigned char ADJAZENZ[knoten+1];
typedef unsigned long *NAUTYGRAPH;
typedef struct le { NAUTYGRAPH nautyg;
		    int knotenzahl;
		    struct le *smaller;
		    struct le *larger; } LISTENTRY;


int graphenzahl=0, noniso, debugzaehler=0, ausgabezaehler=0;
LISTENTRY *arbeitsliste;
GRAPH global_puffer_gr;

int structureCount = 0;
short endian = LITTLE_ENDIAN; // defines which endian should be used while exporting pregraph code

//8-bit popcount lookup table

int bitcounts[256] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
                      1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
                      1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
                      2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
                      1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
                      2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
                      2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
                      3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};

/*extern UPROC distances();*/

void schreibegraph(g)
GRAPH g;
{
int   x,y, unten,oben,maxval,maxkn;


for (maxval=0, x=1; x<=g[0][0]; x++)
   while (g[x][maxval]!=leer) maxval++;

//for (maxkn=knoten; g[maxkn][0]==leer; maxkn--);
maxkn=g[0][0];

fprintf(stderr,"\n\n ");

fprintf(stderr,"|%2d",maxkn);

for(x=1; (x <= maxkn)&&(x<=24); x++)  fprintf(stderr,"|%2d",x); fprintf(stderr,"|\n");

fprintf(stderr," ");

for(x=0; (x <= maxkn)&&(x<=24); x++) fprintf(stderr,"|==");    fprintf(stderr,"|\n");

for(x=0; x < maxval; x++)
 {
  fprintf(stderr," |  ");
  for(y=1; (y<=maxkn)&&(y<=24); y++)
      if (g[y][x] ==leer) fprintf(stderr,"|  "); else fprintf(stderr,"|%2d",g[y][x]);
      fprintf(stderr,"|\n");
 }

unten=25; oben=48;

while(maxkn>=unten)
{
fprintf(stderr,"\n");

fprintf(stderr,"    ");

for(x=unten; (x <= maxkn)&&(x<=oben); x++)  fprintf(stderr,"|%2d",x); fprintf(stderr,"|\n");

fprintf(stderr,"    ");

for(x=unten; (x <= maxkn)&&(x<=oben); x++) fprintf(stderr,"|==");    fprintf(stderr,"|\n");

for(y=0; y < maxval; y++)
 {
  fprintf(stderr,"    ");
  for(x=unten; (x <= maxkn)&&(x<=oben); x++)
      if (g[x][y]==leer) fprintf(stderr,"|  "); else fprintf(stderr,"|%2d",g[x][y]);
      fprintf(stderr,"|\n");
 }
unten += 24; oben += 24;
}

}


/**********UMWANDELN****************************************************/

umwandeln(g,nautyg,m)

GRAPH g;
NAUTYGRAPH nautyg;
int m; /* die fuer nauty kanonische m-Variable */

/* wandelt einen graphen in der normalen darstellung in einen fuer nauty
  passenden graphen um */
/* alle knotennamen muessen fuer nauty um 1 nach unten verschoben werden */

{

int i,j;


for (i=0; i<g[0][0]*m; i++) nautyg[i]=0;
for (i=0; i<g[0][0]; i++) { 
                          for (j=0; g[i+1][j]!=leer; j++) 
                                   ADDELEMENT(nautyg+(m*i),g[i+1][j]-1);
		         }
}


/**************************EINBAUEN********************************/
void einbauen(LISTENTRY *el, NAUTYGRAPH canong,int m, int knotenzahl)

{

debugzaehler++;
     el->nautyg= canong;
     el->knotenzahl=knotenzahl;
     el->smaller=nil;
     el->larger=nil;
   }



/**************************IN_LISTE********************************/

void in_liste(LISTENTRY *el, NAUTYGRAPH canong,int m, int knotenzahl, int *test)
/* schaut nach, ob canong schon in der Liste ist und schreibt ihn eventuell
  rein */
{ int compare;
 int n;



 compare = knotenzahl - el->knotenzahl;
 if (compare==0)
   compare=memcmp(canong,el->nautyg,m*knotenzahl*sizeof(unsigned long));
 if (compare==0) *test=0;
 else if (compare<0) 
   { if (el->smaller==nil)
	{ *test=1; 
	  el->smaller=(LISTENTRY *)malloc(sizeof(LISTENTRY));
	  if (el->smaller==nil) 
	    { fprintf(stderr,"Can not get more memory !\n"); exit(99); }
	  einbauen(el->smaller,canong,m,knotenzahl);
	}
     else in_liste(el->smaller,canong,m,knotenzahl,test);
   }
 else /* compare > 0 */
   { if (el->larger==nil)
	{ *test=1; 
	  el->larger=(LISTENTRY *)malloc(sizeof(LISTENTRY));
	  if (el->larger==nil) 
	    { fprintf(stderr,"Can not get more memory !\n"); exit(99); }
	  einbauen(el->larger,canong,m,knotenzahl);
	}
    else in_liste(el->larger,canong,m,knotenzahl,test);
   }

}




/**************************NEU*************************************/

int neu(GRAPH gr, LISTENTRY **liste)
/* Entscheidet, ob ein graph schon in der Liste behandelter Graphen ist 
  und fuegt ihn ein, wenn nicht */
{
NAUTYGRAPH nautyg, canong;
nvector lab[knoten], ptn[knoten], orbits[knoten];
static DEFAULTOPTIONS(options);
statsblk(stats);
setword workspace[100*knoten];
int m,n,test,knotenzahl;

knotenzahl=gr[0][0];
/*options.invarproc= &distances;*/
options.getcanon=TRUE;
options.writeautoms=FALSE;
options.writemarkers=FALSE;

if ((gr[0][0]%32)==0) m=gr[0][0]/32; else m=gr[0][0]/32+1;

nautyg= (NAUTYGRAPH)calloc(m*knotenzahl,sizeof(unsigned long));
canong= (NAUTYGRAPH)calloc(m*knotenzahl,sizeof(unsigned long));
if ((nautyg==nil) || (canong==nil) )
	    { fprintf(stderr,"Can not get more memory (1)!\n"); exit(99); }

umwandeln(gr,nautyg,m);
nauty(nautyg,lab,ptn,NILSET,orbits,&options, &stats,workspace,100,
                                                  m,gr[0][0],canong); 

/*
for (n=0; n<gr[0][0]; n++) fprintf(stderr," %d ",canong[n]);
fprintf(stderr,"\n");
*/

if ((*liste)==nil)
	{ test=1; 
	  *liste =(LISTENTRY *)malloc(sizeof(LISTENTRY));
	  if (*liste==nil) 
	    { fprintf(stderr,"Can not get more memory (0)!\n"); exit(99); }
	  einbauen(*liste,canong,m,knotenzahl); 
	}
else in_liste(*liste,canong,m,gr[0][0],&test);

free(nautyg); 
if (test==0) free(canong); /* sonst wurde er in die Liste geschrieben */

return(test);
}


/****************************EINFUGEN****************************************/

einfugen (gr,adj,v,w)
GRAPH gr;
unsigned char adj[knoten+1];
unsigned char v, w;
/* Fuegt die Kante (v,w) in den Graphen graph ein. Dabei wird aber davon */
/* ausgegangen, dass in adj die wirklich aktuellen werte fuer die */
/* Adjazenzen stehen. Die adjazenzen werden dann aktualisiert. */

{
gr[v][adj[v]]=w;
gr[w][adj[w]]=v;
adj[v]++;
adj[w]++;
}


/**************************DECODIERE*****************************************/


decodiere(unsigned char *code,GRAPH graph,ADJAZENZ adj,int codelaenge)

{
int i,j;
unsigned char knotenzahl;

/*for(i=0;i<codelaenge;i++) printf(" %d ",code[i]); printf("\n");*/

graph[0][0]=knotenzahl=code[0];

for (i=1; i<= knotenzahl; i++) { adj[i]=0;
                                for (j=0; j<MAXVAL; j++) graph[i][j]=leer;
                              }
for (j=1; j<knoten; j++) graph[0][j]=0;


j=1;

for (i=1; i<codelaenge; i++) 
           { if (code[i]==0) j++; else einfugen(graph,adj,j,code[i]); }
}




/**********************************CODIERE********************************/

void codiere_und_schreibe(gr)
GRAPH gr;

{
int i,j,codestelle;
unsigned char code[knoten*knoten];

/* Codierung: Erstes byte: knotenzahl danach die zu knoten 1 adjazenten
  ALLE GROESSEREN vertices, beendet durch 0, danach die zu knoten 2 
  adjazenten GROESSEREN .... jeweils beendet durch 0. Da der letzte Knoten
  nie zu groesseren adjazent sein kann, braucht seine (immer leere) Liste 
  nicht durch 0 abgeschlossen werden*/


code[0]=gr[0][0];

for (codestelle=1; gr[1][codestelle-1]!=leer; codestelle++) 
                        code[codestelle]=gr[1][codestelle-1]; 
/* 0 ist das Trennzeichen fuer den naechsten Code */
/* Gesamtcodelaenge: Kantenzahl+knotenzahl      */

for (j=2; j<=gr[0][0]; j++)
{ code[codestelle]=0; codestelle++;
 for (i=0; gr[j][i]!=leer; i++) 
                    if (gr[j][i]>j) 
                           { code[codestelle]=gr[j][i]; codestelle++; }
}

fwrite(code,sizeof(unsigned char),codestelle,stdout);

}


/***********************NAUTY_TO_GRAPH*************************/

void nauty_to_graph(NAUTYGRAPH nautyg, GRAPH gr, int knotenzahl)
{
int i,j,m;
ADJAZENZ adj;


if ((knotenzahl%32)==0) m=knotenzahl/32; else m=knotenzahl/32+1;

for (i=0; i<=knoten; i++) 
  { for (j=0;j<MAXVAL; j++) gr[i][j]=leer;
    adj[i]=0; }

gr[0][0]=knotenzahl;

for (i=0; i<knotenzahl; i++)
   { j = -1;
     while ((j = nextelement(nautyg+m*i,m,j)) >= 0)
	  if (j>i) { einfugen(gr,adj,i+1,j+1); }
   }

}

/*********************AUSGABE***********************************/

void ausgabe(LISTENTRY *liste)

{ 

/* GRAPH gr;  muss global gehandhabt werden, um beim iterierten aufruf Platz zu
sparen */
if (liste==nil) return;

ausgabezaehler++;

if (liste->larger != nil) ausgabe(liste->larger);

//nauty_to_graph(liste->nautyg,global_puffer_gr,liste->knotenzahl);
noniso++;
//codiere_und_schreibe(global_puffer_gr);

if (liste->smaller != nil) ausgabe(liste->smaller); 

}



/****************************LESE_MULTICODE************************/

int lese_multicode(unsigned char**code, int *codelaenge, FILE *fil)

/* Liest den code und gibt EOF zurueck, wenn das Ende der Datei erreicht
  ist, 1 sonst. Der Speicher fuer den code wird alloziert, falls noetig,
  was entschieden wird anhand der lokalen Variablen maxknotenzahl */
{
static int maxknotenzahl= -1;
int codel, gepuffert=0;
int knotenzahl,a,b,nuller;
unsigned char ucharpuffer;

if ((knotenzahl=getc(fil))==EOF) return EOF;
if (knotenzahl==0) { fprintf(stderr,"Umschaltung auf short noch nicht implementiert.\n");
                    exit(0);
                   } 
nuller=0; codel=1;
if (knotenzahl=='>') /* koennte ein header sein -- oder 'ne 62, also ausreichend fuer
			     unsigned char */
     { gepuffert=1;
	a=getc(fil);
	if(a==0) nuller++; 
	b=getc(fil);
	if(b==0) nuller++; 
	/* jetzt wurden 3 Zeichen gelesen */
	if ((a=='>') && (b=='m')) /*garantiert header*/
	  { 
	    gepuffert=0;
	    while ((ucharpuffer=getc(fil)) != '<');
	    /* noch zweimal: */ ucharpuffer=getc(fil); 
	    if (ucharpuffer!='<') { fprintf(stderr,"Problems with header -- single '<'\n"); exit(1); }
	    if ((knotenzahl=getc(fil))==EOF) return EOF;
	    /* kein graph drin */
	  }
	/* else kein header */
     }


if (knotenzahl > maxknotenzahl)
 { if (*code) free(*code);
   *code=(unsigned char *)malloc((knotenzahl*(knotenzahl-1)/2+knotenzahl)*sizeof(unsigned char));
   if (code==NULL) { fprintf(stderr,"Do not get memory for code\n"); exit(0); }
   maxknotenzahl=knotenzahl;
 }

(*code)[0]=knotenzahl; if (gepuffert) { codel=3; (*code)[1]=a; (*code)[2]=b; }

while (nuller<knotenzahl-1)
 { if (((*code)[codel]=getc(fil))==0) nuller++;
   codel++; }

*codelaenge=codel;
return 1;
}

const char *byte_to_binary(int x, int length)
{
    static char b[10];
    b[0] = '\0';

    int z;
    for (z = 1<<(length-1); z > 0; z >>= 1){
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

char write_2byte_number(FILE *f, unsigned short n, short writeEndian) {
    if (writeEndian == BIG_ENDIAN) {
        fprintf(f, "%c%c", n / 256, n % 256);
    } else {
        fprintf(f, "%c%c", n % 256, n / 256);
    }
    return (ferror(f) ? 2 : 1);
}

void printset(GRAPH graph,ADJAZENZ adj, int rank){
    int order = graph[0][0];
	int hypercubeOrder = 1 << rank;
	int completeGraphOrder = order - hypercubeOrder;
	int vertices[completeGraphOrder];
	
	//find the elements of the set
	int i, j=0, k;
	for(i=1; i<=order; i++){
	    if(adj[i]==1){
		    vertices[j++] = graph[i][0]-1;
		}
	}
	
	//construct the distance matrix
	int extraVertices = 0;
	int distances[completeGraphOrder][completeGraphOrder];
	for(i = 0; i<completeGraphOrder; i++){
	    distances[i][i] = 0;
		for(j=i+1; j<completeGraphOrder; j++){
			//compute hamming distance
			int xor = vertices[i] ^ vertices[j];
			int distance = bitcounts[xor];
			distances[i][j] = distances[j][i] = distance;
			extraVertices += distance - 1;
		}
	}
	if(completeGraphOrder == 3){
		extraVertices += 3;
	}
	
	/*
	fprintf(stderr, "Extra vertices: %d\n", extraVertices);
	
	for(i = 0; i<completeGraphOrder; i++){
		fprintf(stderr, "%d ", vertices[i]);
	}
	fprintf(stderr, "\n\n");
	
	for(i = 0; i<completeGraphOrder; i++){
		for(j=0; j<completeGraphOrder; j++){
			fprintf(stderr, "%d ", distances[i][j]);
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
	*/
	
	//construct new adjacency list
	int adjList[completeGraphOrder + extraVertices][completeGraphOrder+1];
	int degree[completeGraphOrder + extraVertices];
	for(i=0; i<completeGraphOrder + extraVertices; i++) degree[i]=0;
	
	int extraVertex = completeGraphOrder;
	for(i = 0; i<completeGraphOrder; i++){
		for(j=i+1; j<completeGraphOrder; j++){
			if(distances[i][j] == 1){
				adjList[i][degree[i]] = j;
				degree[i]++;
			} else {
				adjList[i][degree[i]] = extraVertex;
				degree[i]++;
				for(k=0; k<distances[i][j]-2; k++){
					adjList[extraVertex][0] = extraVertex+1;
					degree[extraVertex]++;
					extraVertex++;
				}
				adjList[j][degree[j]] = extraVertex;
				degree[j]++;
				extraVertex++;
			}
		}
	}
	if(completeGraphOrder == 3){
		for(i=0; i<completeGraphOrder; i++){
			adjList[i][degree[i]++] = extraVertex++;
		}
	}
	
	//output multicode
	structureCount++;
	
	FILE *f = stdout;
	
	//fprintf(stderr, "%3d : ", structureCount);
	
    if (structureCount == 1) { //if first graph
        fprintf(f, ">>multi_code %s<<", (endian == LITTLE_ENDIAN ? "le" : "be"));
    }
    if (completeGraphOrder + extraVertices <= UCHAR_MAX) {
        fprintf(f, "%c", (unsigned char) (completeGraphOrder + extraVertices));
		//fprintf(stderr, "%d ", completeGraphOrder + extraVertices);
    } else {
        fprintf(f, "%c", 0);
        /* big graph */
        if (write_2byte_number(f, (unsigned short) (completeGraphOrder + extraVertices), endian) == 2) {
            return;
        }
    }
	
	for(i=0; i<completeGraphOrder + extraVertices - 1; i++){
		for(j=0; j<degree[i]; j++){
			if(completeGraphOrder + extraVertices <= UCHAR_MAX){
				fprintf(f, "%c", (unsigned char)(adjList[i][j]+1));
				//fprintf(stderr, "%d ", adjList[i][j]+1);
			} else {
				if (write_2byte_number(f, adjList[i][j]+1, endian) == 2) {
					return;
				}
			}
		}
		
        //closing 0
        if (completeGraphOrder + extraVertices <= UCHAR_MAX) {
            fprintf(f, "%c", 0);
            //fprintf(stderr, "%d ", 0);
        } else {
            if (write_2byte_number(f, 0, endian) == 2) {
                return;
            }
        }
	}
	//fprintf(stderr, "\n");

}



/******************************MAIN************************************/


main(argc,argv)

int argc;
char *argv[];


{
GRAPH gr;
ADJAZENZ adj;
unsigned char *code=NULL;
int  zaehlen=0, number=0, after=0, codelaenge;
int dreier=0, vierer=0, dummy, writethem, info;
int i,j, nuller;
int anzB=0, anzC=0, anzS=0;



arbeitsliste=nil;

if (argc != 2) 
{ fprintf(stderr,"usage: hcs_non_iso n");
 fprintf(stderr,"It reads multigraph codes from stdin, and writes them --\n");
 fprintf(stderr,"ONLY FOR SIMPLE GRAPHS !! \n");
     exit(0); }

//parse the rank
int rank = strtol(argv[1], NULL, 10);

zaehlen=noniso=0;

while(lese_multicode(&code, &codelaenge, stdin) != EOF)
 {
   zaehlen++; 
   decodiere(code,gr,adj,codelaenge);
   int isNew = neu(gr,&arbeitsliste);
//   if (isNew && writethem) fwrite(code,sizeof(unsigned char),codelaenge,stdout);
   if(isNew){
       printset(gr,adj,rank);
   }
 }

  if (arbeitsliste != nil) ausgabe(arbeitsliste);

fprintf(stderr,"Gelesen: %d Graphen, davon %d paarweise nicht isomorph.\n",zaehlen,noniso);

return(0);

}
