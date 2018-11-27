// Copyright 2018 Christopher D. Rosin
// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

// input/output example
struct example {
  long long int target[MAXY*MAXXDIM];
};
long long int probleminitscore;
long long int bestcycles[NUMINSTRUCTIONS]; // best cycle count at each simplicity

// Read target image (1 training example), and setup parameters.
void setupdata(char *examplefile,struct example examples[MAXEXAMPLES],int *numtrain,int *numtest) {
  (*numtrain) = 1;
  (*numtest) = 0;
  probleminitscore = 0;
  FILE *fp = fopen(examplefile,"r");
  for(int y=0;y<MAXY;y++) {
    for(int x=0;x<MAXX;x++) {
      int temp;
      int r=fscanf(fp,"%d",&temp);
      examples[0].target[y*MAXXDIM+x] = temp;
      if(temp==0) probleminitscore++;
  } }
  fclose(fp);

  VBITS=1;
  int v = VVALS;
  while(v>>=1) VBITS++;
  printf("VBITS %d\n",VBITS);

  for(int i=0;i<NUMINSTRUCTIONS;i++) bestcycles[i] = -1;

  for(int i=0;i<MAXCODES;i++) {
    int o = (i>>VBITS);
    if(o<OVALS) {
      int v = (i&VMASK);
      if((o>=JMP)&&(o<=JLZ)) sprintf(istr[i],"%s %d",oname[o],((int) (((double) v)*((double) (((double) NUMINSTRUCTIONS)/ ((double) VVALS))))));
	else if(o==MOVAD) sprintf(istr[i],"MOV ACC,DOWN");
	else if(o==MOVCD) sprintf(istr[i],"MOV %d,DOWN",v+IMMEDIATECONSTANTOFFSET);
	else if(o==MOVCA) sprintf(istr[i],"MOV %d,ACC",v+IMMEDIATECONSTANTOFFSET);
	else if((o==ADDA)||(o==SUBA)) sprintf(istr[i],"%s ACC",oname[o]);
	else if((o==ADDC)||(o==SUBC)) sprintf(istr[i],"%s %d",oname[o],v+IMMEDIATECONSTANTOFFSET);
	else sprintf(istr[i],"%s",oname[o]);
} } }

// example size for computing bounds
long long int examplesize(struct example *ex) { return((long long int) (MAXX*MAXY)); }

int examplemaxscore(struct example *ex) { return(TARGETSCORE); }

void printcodes(int *codes) {
  for(int i=0;i<NUMINSTRUCTIONS;i++) printf("%2d: %-13s | %d\n",i,istr[codes[i]],codes[i]);
  printf("\n");
  fflush(stdout);
}

// softasm_setup has already been called on current program; now evaluate one example.  Return score, and set correctflag if completely correct.
int exampleeval(struct example *ex,long long int bound,int *correctflag) {
  long long int img[MAXY*MAXXDIM];
  for(int i=0;i<MAXY*MAXXDIM;i++) img[i]=0;
  long long int curcyclecountbound = bound;
  long long int ret = fptr(&curcyclecountbound,img,ex->target,probleminitscore);
  int numcorrect=ret;
  (*correctflag)=0;
  if(ret==TARGETSCORE) {
    (*correctflag)=1;
    int thiscyclecount = bound-curcyclecountbound;
    int cursimplicity = 0;  		
    for(int i=0;i<NUMINSTRUCTIONS;i++) cursimplicity+=((savecodes[i]>>VBITS)==NOP);
    if((bestcycles[cursimplicity]<0)||(thiscyclecount<bestcycles[cursimplicity])) {
      bestcycles[cursimplicity] = thiscyclecount;
      printf("correct solution with new best cyclecount %d at simplicity %d\n",thiscyclecount,cursimplicity);
      printcodes(savecodes);
  } }
  return(numcorrect);  		      
}
