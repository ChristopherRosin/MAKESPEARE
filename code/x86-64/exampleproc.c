// Copyright 2018 Christopher D. Rosin
// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#define MAXLENGTH 100001

// input/output example
struct example {
  long long int in_mem[MAXLENGTH];
  long long int in_reg[NUMREGISTERS];
  long long int out_mem[MAXLENGTH];
  long long int out_returnval;    // Function's scalar return value.
  int in_mem_length;              // First location is in_mem[0], last location is in_mem[in_mem_length-1].  No memory if in_mem_length==0.
  int out_mem_start;  
  int out_mem_length;             // First location is out_mem[out_mem_start], last is out_mem[out_mem_start+out_mem_length-1].  May be subrange of input (the rest of the output is then don't-care).  No output memory if out_mem_length==0.
  int out_returnvalflag;          // If 0, out_returnval is ignored and return val is not checked.
};

// Read training & test sets, and setup parameters.
void setupdata(char *examplefile,struct example examples[MAXEXAMPLES],int *numtrain,int *numtest) {
  int maxinputlength = 0;
  FILE *fp = fopen(examplefile,"r");
  char tmpbuf[10000];
  char *dum = fgets(tmpbuf,sizeof(tmpbuf),fp); // dump header line
  (*numtrain) = 0;
  (*numtest) = 0;
  int r;
  do {
    int testflag;
    r=fscanf(fp,"%d",&testflag);
    if(r!=EOF) {
      if((testflag==0)&&((*numtest)>0)) ERROREXIT("FATAL ERROR: training example after %d test examples\n",(*numtest));
      int i = (*numtrain)+(*numtest);
      for(int j=0;j<NUMREGISTERS;j++) r=fscanf(fp,"%lld",&(examples[i].in_reg[j]));
      r=fscanf(fp,"%d",&(examples[i].in_mem_length));
      if(examples[i].in_mem_length>MAXLENGTH) ERROREXIT("FATAL ERROR: MAXLENGTH %d, read %d\n",MAXLENGTH,examples[i].in_mem_length);
      if(examples[i].in_mem_length>maxinputlength) maxinputlength=examples[i].in_mem_length;
      for(int j=0;j<examples[i].in_mem_length;j++) r=fscanf(fp,"%lld",&(examples[i].in_mem[j]));
      r=fscanf(fp,"%d",&(examples[i].out_returnvalflag));
      r=fscanf(fp,"%lld",&(examples[i].out_returnval));
      r=fscanf(fp,"%d",&(examples[i].out_mem_start));
      r=fscanf(fp,"%d",&(examples[i].out_mem_length));
      if(examples[i].out_mem_start+examples[i].out_mem_length>MAXLENGTH) ERROREXIT("FATAL ERROR: out MAXLENGTH %d, read %d %d\n",MAXLENGTH,examples[i].out_mem_start,examples[i].out_mem_length);
      for(int j=examples[i].out_mem_start;j<examples[i].out_mem_start+examples[i].out_mem_length;j++) r=fscanf(fp,"%lld",&(examples[i].out_mem[j]));      
      if(testflag==0) (*numtrain)++;
        else (*numtest)++;
      if(((*numtrain)+(*numtest))>=MAXEXAMPLES) ERROREXIT("FATAL ERROR: MAXEXAMPLES reached %d %d %d\n",(*numtrain),(*numtest),MAXEXAMPLES);
    }      
  } while(r!=EOF);
  fclose(fp);
  printf("got %d train, %d test\n",*numtrain,*numtest);

  VVALS = NUMIMMEDIATECONSTANTS + (2-(maxinputlength==0))*NUMREGISTERS;             // maxinputlength==0 indicates scalar-only inputs
  IVALS = (((0x1<<OBITS)-(maxinputlength==0))*((0x1<<VBITS)-(maxinputlength==0)));
  for(int i=0;i<MAXCODES;i++) {
    int o = (i>>VBITS);
    if(o<OVALS) {
      int v = (i&VMASK);
      if((o>=JMP)&&(o<=JG)) sprintf(istr[i],"%s %d",oname[o],v*(NUMINSTRUCTIONS/VVALS));
        else if((v<NUMIMMEDIATECONSTANTS)&&(o==ARG)) sprintf(istr[i]," ");
        else if(v<NUMIMMEDIATECONSTANTS) sprintf(istr[i],"%s %d",oname[o],immediateconstants[v]);
        else if(v-NUMIMMEDIATECONSTANTS<NUMREGISTERS) sprintf(istr[i],"%s r%d",oname[o],registers[v-NUMIMMEDIATECONSTANTS]);
        else sprintf(istr[i],"%s [r%d]",oname[o],registers[v-NUMIMMEDIATECONSTANTS-NUMREGISTERS]);
} } }

// example size for computing bounds
long long int examplesize(struct example *ex) {
  long long int n = ex->in_reg[0]; 
  if(ex->in_mem_length>0) n=ex->in_mem_length;
  return(n);
}

int examplemaxscore(struct example *ex) {
  return(ex->out_returnvalflag + ex->out_mem_length);
}

void printcodes(int *codes) {
  int suppress[NUMINSTRUCTIONS];  // suppress unnecessary ARG
  int neededflag = 0;
  for(int i=NUMINSTRUCTIONS-1;i>=0;i--) {
    int o = (codes[i]>>VBITS);
    if((o==ARG)&&(neededflag==0)) suppress[i]=1;
      else suppress[i]=0;
    if((neededflag==1)&&(o==ARG)&&((codes[i]&VMASK)>=NUMIMMEDIATECONSTANTS)) neededflag=0;
    if(o<=TEST) neededflag=1;
  }
  int i=0;
  while((i<NUMINSTRUCTIONS)&&((suppress[i]==1)||((codes[i]>>VBITS)!=ARG))) i++;
  if((i<NUMINSTRUCTIONS)&&((codes[i]>>VBITS)==ARG)&&((codes[i]&VMASK)==(NUMIMMEDIATECONSTANTS+3))) suppress[i]=1; // default r0 ARG
  for(int i=0;i<NUMINSTRUCTIONS;i++) {
    if(suppress[i]==0) printf("%2d: %-13s | %d\n",i,istr[codes[i]],codes[i]);
      else printf("%2d:               | %d\n",i,codes[i]);
  }
  printf("\n");
  fflush(stdout);
}

// softasm_setup has already been called on current program; now evaluate one example.  Return score, and set correctflag if completely correct.
int exampleeval(struct example *ex,long long int bound,int *correctflag) {
  long long int tempin[MAXLENGTH];
  for(int j=0;j<ex->in_mem_length;j++) tempin[j]=ex->in_mem[j];
  long long int curloopcountbound = bound;
  long long int ret = fptr(ex->in_reg[0],ex->in_reg[1],ex->in_reg[2],ex->in_reg[3],ex->in_reg[4],ex->in_reg[5],tempin,&curloopcountbound);
  int score = 0;
  (*correctflag) = 1;
  if(ex->out_returnvalflag) if(ex->out_returnval!=ret) (*correctflag)=0; else score++;
  for(int j=0;j<ex->out_mem_length;j++) if(ex->out_mem[ex->out_mem_start+j]!=tempin[ex->out_mem_start+j]) (*correctflag)=0; else score++;
  return(score);
}
