// Copyright 2018 Christopher D. Rosin
// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#define MAXEXAMPLES 2201
#define BOUNDPARMS 4
#define ERROREXIT(STRING, ...) do { printf(STRING,__VA_ARGS__); fflush(stdout); exit(1); } while (0)
#define RNGOFF(m,v) ((m)+1-(v))
#define RNGMID(x,m,v) ((((x)&(m))>((m)>>1))&&(((x)&(m))<=(((m)>>1)+RNGOFF(m,v))))

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"
#include "softasm.h"
#include "exampleproc.c"

// apply trainbound and testbound to examples' sizes
void computebounds(struct example examples[MAXEXAMPLES],int numtrain,int numtest,long long int bound[MAXEXAMPLES],double trainbound[BOUNDPARMS],double testbound[BOUNDPARMS]) {
  for(int i=0;i<numtrain+numtest;i++) {
    long long int n = examplesize(&(examples[i]));
    if(i<numtrain) {
      bound[i] = trainbound[0]*pow(n,trainbound[1])*pow(log(n+1)/log(2.0),trainbound[2])+trainbound[3]+1.001; // ensure it is rounded up and not zero
      printf("example %d size %lld training bound %lld\n",i,n,bound[i]);
    } else {
      bound[i] = testbound[0]*pow(n,testbound[1])*pow(log(n+1)/log(2.0),testbound[2])+testbound[3]+1.001; 
      printf("example %d size %lld test bound %lld\n",i,n,bound[i]);
} } }

// assuming sequential eval of training examples, compute max remaining score as of each example; for shortcutting
void computeevalshortcuts(struct example examples[MAXEXAMPLES],int numtrain,int maxremscore[MAXEXAMPLES]) {
  maxremscore[numtrain-1] = examplemaxscore(&(examples[numtrain-1]));
  for(int i=numtrain-2;i>=0;i--) maxremscore[i] = maxremscore[i+1] + examplemaxscore(&(examples[i]));
}

// random opcode or operand
int randop(int m,int v) {
  int op = random()&m;
  while(RNGMID(op,m,v)) op=(random()&m);
  if(op>(m>>1)) op-=RNGOFF(m,v);
  return(op);
}

// random opcode and operand
int randcode() {
  int code = random()%IVALS;
  while((RNGMID((code>>VBITS),OLMASK,OVALS))||(RNGMID(code,VMASK,VVALS))) code=random()%IVALS;
  if((code&OMASK)>(OMASK>>1)) code-=(RNGOFF(OLMASK,OVALS)<<VBITS);
  if((code&VMASK)>(VMASK>>1)) code=((code&OMASK)|((code&VMASK)-RNGOFF(VMASK,VVALS)));
  return(code);
}

// initialize random program
void initialize(int codes[NUMINSTRUCTIONS]) { for(int i=0;i<NUMINSTRUCTIONS;i++) codes[i]=randcode(); }

// generate a modified program
void perturb(int codes[NUMINSTRUCTIONS],int swapp10000,int doublep10000,int copyp10000) {
  if((random()%10000)<swapp10000) {
    int a=random()%NUMINSTRUCTIONS, b=random()%NUMINSTRUCTIONS;
    while(a==b) b=random()%NUMINSTRUCTIONS;
    int tmp = codes[a];
    codes[a] = codes[b];
    codes[b] = tmp;
  } else {
    int doubleflag = ((random()%10000)<doublep10000);
    for(int i=0;i<=doubleflag;i++) {
      int loc = random()%NUMINSTRUCTIONS;
      if((random()%10000)<copyp10000) {
	if((random()%10000)<copyp10000) {
	  codes[loc]=(((codes[random()%NUMINSTRUCTIONS])&VMASK)|((codes[random()%NUMINSTRUCTIONS])&OMASK));
	} else {
	  if(OVALS>OLMASK) codes[loc]=(((codes[random()%NUMINSTRUCTIONS])&VMASK)|((random()&OLMASK)<<VBITS));
	    else codes[loc]=(((randop(OLMASK,OVALS))<<VBITS)|((codes[random()%NUMINSTRUCTIONS])&VMASK));
	}
      } else {
	if((random()%10000)<copyp10000) {
	  if(VVALS>VMASK) codes[loc]=(((codes[random()%NUMINSTRUCTIONS])&OMASK)|((random()&VMASK)));
	    else codes[loc]=((randop(VMASK,VVALS))|((codes[random()%NUMINSTRUCTIONS])&OMASK));
	} else {
	  codes[loc] = randcode();
} } } } }

// Run program codes[] on numex's ex[] starting from startexample.  Return score, and set totcorrect #fully-correct examples.  May shortcut & terminate early via thresh and maxremscore.
int eval(struct example ex[MAXEXAMPLES],int startexample,int numex,long long int bound[MAXEXAMPLES],int *totcorrect,int codes[NUMINSTRUCTIONS],int maxremscore[MAXEXAMPLES],int thresh) {
  softasm_setup(codes);
  (*totcorrect) = 0;
  int correctflag, score=0;
  for(int i=startexample;i<startexample+numex;i++) {
    score += exampleeval(&(ex[i]),bound[i],&correctflag);
    (*totcorrect) += correctflag;
    if((thresh>0)&&(i<startexample+numex-1)&&(score+maxremscore[i+1]<thresh)) break;
  }
  softasm_cleanup();
  return(score);
}

// Count occurrences of distinguished opcode N, which does not produce executable code.
int simplicityscore(int codes[NUMINSTRUCTIONS]) {
  int numns = 0;
  for(int i=0;i<NUMINSTRUCTIONS;i++) if((codes[i]>>VBITS)==N) numns++; 
  return(numns);
}

// Delayed-acceptance hillclimbing (unless daflag==0; then basic hillclimbing).
void search(struct example examples[MAXEXAMPLES],int numtrain,int numtest,long long int bound[MAXEXAMPLES],int maxremscore[MAXEXAMPLES],long long int periodprogs,int maxperiods,int swapp10000,int doublep10000,int copyp10000,int daflag) {
  int bestscore=0, thresh=0, curthresh=0; // "curthresh" is only used by basic hillclimbing.
  int codes[NUMINSTRUCTIONS],prevcodes[NUMINSTRUCTIONS],bestcodes[NUMINSTRUCTIONS],numperiods=0;
  for(long long int n=0;numperiods<maxperiods;n++) { // may exit via break
    if(bestscore==0) initialize(codes);
    else {
      for(int i=0;i<NUMINSTRUCTIONS;i++) prevcodes[i]=codes[i];
      perturb(codes,swapp10000,doublep10000,copyp10000);
    }
    int simplicity = simplicityscore(codes);
    int totcorrect, numtestcorrect, score=0;
    if(simplicity>=thresh-maxremscore[0]) {  // Otherwise this cannot possibly be accepted.  Note maxremscore[0] is fully correct on training set. 
      score = eval(examples,0,numtrain,bound,&totcorrect,codes,maxremscore,((thresh>maxremscore[0]) ? (maxremscore[0]) : thresh));
      if(totcorrect>=numtrain) {
	score += simplicity;
	if(score>bestscore) {
	  bestscore = score;
	  int dum = eval(examples,numtrain,numtest,bound,&numtestcorrect,codes,maxremscore,0); // Final 0 means maxremscore is unused here.
	  printf("%lld new bestscore %d totcorrect %d at simplicity %d.  Fully correct on training set.  numtestcorrect %d\n",n,score,totcorrect,simplicity,numtestcorrect);
	  printcodes(codes);
	}
      } else {
	if(score>bestscore) {
	  bestscore = score;
	  printf("%lld new bestscore %d totcorrect %d\n",n,bestscore,totcorrect);
	  printcodes(codes);
    } } }

    if(daflag) { // delayed acceptance
      if(score>=bestscore) for(int i=0;i<NUMINSTRUCTIONS;i++) bestcodes[i]=codes[i];
        else if(score<thresh) for(int i=0;i<NUMINSTRUCTIONS;i++) codes[i]=prevcodes[i];
    } else { // basic hillclimbing
      if(score>=bestscore) {
	for(int i=0;i<NUMINSTRUCTIONS;i++) bestcodes[i]=codes[i];
	curthresh = score;
      } else if(score>curthresh) {
	curthresh = score;
      } else if(score<curthresh) {
	for(int i=0;i<NUMINSTRUCTIONS;i++) codes[i]=prevcodes[i];
    } }
    
    if((n%periodprogs)==(periodprogs-1)) {
      if(thresh==bestscore) {
	printf("%lld completed (no progress), thresh at %d\n",n,thresh);
	break;
      } else {
	for(int i=0;i<NUMINSTRUCTIONS;i++) codes[i]=bestcodes[i];
	thresh = bestscore;
	printf("%lld updated thresh to %d\n",n,thresh);
	fflush(stdout);
	numperiods++;
} } } }

void main(int argc,char* argv[]) {
  int numtrain,numtest,maxremscore[MAXEXAMPLES];  
  long long int bound[MAXEXAMPLES];
  unsigned int seed;
  double testbound[BOUNDPARMS], trainbound[BOUNDPARMS]={2.0,1.0,0.0,0.0};
  long long int periodprogs=75000;
  int maxperiods=10000, numruns=1, daflag=1, swapp10000=1000, doublep10000=9000, copyp10000=5000;
  double tempinput;
  struct example *examples = malloc(MAXEXAMPLES*sizeof(struct example));
  
  printf("Usage: makespeare seed problemfile a(2.0) b(1.0) c(0) d(0) I(75k) maxperiods(10000;terminate naturally) numruns(1) a'(a) b'(b) c'(c) d'(d) swapP(0.1) doubleP(0.9) copyP(0.5) delayedacceptanceflag(1)\n");
  printf("Parameters seed and problemfile are required; others parameters optional with defaults shown in parentheses.\n");
  printf("Training time bound for instance size n is a*(n^b)*(lg n)^c+d, and separate testing time bound can be specified with a' b' c' d'.\n\n");
  if(argc<3) ERROREXIT("only %d args; must specify at least seed and problemfile\n",argc-1);
  sscanf(argv[1],"%u",&seed);
  for(int i=0;i<BOUNDPARMS;i++) if(argc>3+i) sscanf(argv[3+i],"%lf",&(trainbound[i]));
  for(int i=0;i<BOUNDPARMS;i++) testbound[i]=trainbound[i];
  if(argc>7) sscanf(argv[7],"%lld",&periodprogs);
  if(argc>8) sscanf(argv[8],"%d",&maxperiods);
  if(argc>9) sscanf(argv[9],"%d",&numruns);
  for(int i=0;i<BOUNDPARMS;i++) if(argc>10+i) sscanf(argv[10+i],"%lf",&(trainbound[i]));
  if(argc>14) { sscanf(argv[14],"%lf",&tempinput); swapp10000=(10000.0*tempinput)+0.5; }
  if(argc>15) { sscanf(argv[15],"%lf",&tempinput); doublep10000=(10000.0*tempinput)+0.5; }
  if(argc>16) { sscanf(argv[16],"%lf",&tempinput); copyp10000=(10000.0*tempinput)+0.5; }
  if(argc>17) sscanf(argv[17],"%d",&daflag);
  printf("got %u %s %lf %lf %lf %lf %lld %d %d %lf %lf %lf %lf %d/10000 %d/10000 %d/10000 %d\n",seed,argv[2],trainbound[0],trainbound[1],trainbound[2],trainbound[3],periodprogs,maxperiods,numruns,testbound[0],testbound[1],testbound[2],testbound[3],swapp10000,doublep10000,copyp10000,daflag);

  srandom(seed);
  for(int i=0;i<numruns;i++) {
    if(numruns>1) {
      seed = random();
      srandom(seed);
      printf("starting run %d with seed %u starting\n",i,seed);
    }
    setupdata(argv[2],examples,&numtrain,&numtest);
    computebounds(examples,numtrain,numtest,bound,trainbound,testbound);
    computeevalshortcuts(examples,numtrain,maxremscore);
    search(examples,numtrain,numtest,bound,maxremscore,periodprogs,maxperiods,swapp10000,doublep10000,copyp10000,daflag);
} }
