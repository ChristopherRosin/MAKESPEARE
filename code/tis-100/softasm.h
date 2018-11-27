/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x64 version 1.4.0
** DO NOT EDIT! The original file is in "softasm-tis100.dasc".
*/

#line 1 "softasm-tis100.dasc"
// Copyright 2018 Christopher D. Rosin
// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdint.h>
#include <sys/mman.h>

#define MAXX 30
#define MAXY 18
#define XBITS 5  // MAXX must be <=2^XBITS.  And the image arrays passed in must be [MAXY][2^XBITS] 
#define MAXXDIM (0x1<<XBITS)

#define TARGETSCORE (MAXX*MAXY)

#define NUMINSTRUCTIONS 15           // number of slots
#define NUMIMMEDIATECONSTANTS 401    // 1999 for full range
#define IMMEDIATECONSTANTOFFSET (-(NUMIMMEDIATECONSTANTS>>1))  // all operands are immediate constants
#define VVALS NUMIMMEDIATECONSTANTS  // "V" for operands
#define VMASK ((0x1<<VBITS)-1)
#define OBITS 4                      // "O" for opcodes
#define OVALS 16                     // must be <= 2^OBITS.  And must match the number of opcodes defined and handled below.
#define OLMASK ((0x1<<OBITS)-1)
#define OMASK ((OLMASK)<<VBITS)
#define IVALS (0x1<<(OBITS+VBITS)) 
#define MAXCODES (0x1<<(OBITS+VBITS))
#define MAXMAXCODES (0x1<<16)
int VBITS;

enum opcodes {NOP,MOVAD,MOVCD,MOVCA,SWP,SAV,NEG,ADDC,ADDA,SUBC,SUBA,JMP,JEZ,JNZ,JGZ,JLZ};
const char* oname[] = {"NOP","MOV","MOV","MOV","SWP","SAV","NEG","ADD","ADD","SUB","SUB","JMP","JEZ","JNZ","JGZ","JLZ"};
char istr[MAXMAXCODES][14];

#define N NOP

// Store needed constants in unused locations in the image memory.
#define CONSTLOC999 (30*8)
#define CONSTLOCNEG999 (31*8)
#define CONSTLOC0 (62*8)
#define CONSTLOC1 (63*8)
#define CONSTLOC2 (94*8)

#define Dst &state

//|.arch x64
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 46 "softasm-tis100.dasc"
//|.actionlist actions
static const unsigned char actions[410] = {
  83,65,84,65,85,65,86,65,87,76,139,23,72,199,134,233,0,0,0,0,72,199,134,233,
  1,0,0,0,72,199,134,233,2,0,0,0,72,199,134,233,231,3,0,0,72,199,134,233,25,
  252,252,252,255,252,255,77,49,252,246,77,49,252,237,73,137,203,72,137,200,
  77,49,228,72,49,201,255,249,255,73,131,252,234,1,15,142,245,255,252,233,245,
  255,73,131,252,254,0,15,132,245,255,73,131,252,254,0,15,133,245,255,73,131,
  252,254,0,15,143,245,255,73,131,252,254,0,15,140,245,255,76,137,252,241,77,
  137,252,238,73,137,205,255,77,137,252,245,255,73,252,247,222,255,73,129,198,
  239,255,73,129,252,254,231,3,0,0,76,15,79,182,233,73,129,252,254,25,252,252,
  252,255,252,255,76,15,76,182,233,255,77,1,252,246,255,73,129,252,238,239,
  255,73,199,198,0,0,0,0,255,73,199,198,237,255,76,137,252,241,255,72,199,193,
  237,255,72,131,252,249,0,76,15,76,166,233,15,140,245,255,73,131,252,252,0,
  76,15,68,193,76,15,68,166,233,15,132,245,255,73,131,252,252,1,76,15,68,201,
  76,15,68,166,233,15,132,245,255,73,129,252,248,239,15,131,245,73,129,252,
  249,239,15,131,245,255,72,131,252,249,4,72,15,79,142,233,255,77,137,207,73,
  193,231,235,77,9,199,255,73,131,192,1,255,74,59,12,252,254,15,132,245,255,
  72,199,195,0,0,0,0,74,59,12,252,250,72,15,68,158,233,73,1,219,74,139,28,252,
  254,74,137,12,252,254,72,199,193,0,0,0,0,74,59,28,252,250,72,15,68,142,233,
  73,41,203,255,73,57,195,15,142,245,76,137,216,72,129,252,248,239,15,132,245,
  255,249,252,233,245,255,249,76,137,23,65,95,65,94,65,93,65,92,91,195,255
};

#line 47 "softasm-tis100.dasc"
//|.define CYCLECOUNTREG, r10
//|.define CYCLECOUNTLOCATIONREG, r7
//|.define ACC, r14
//|.define BAK, r13
//|.define TMP, r1
//|.define TMPB,r3    
//|.define BESTSCORE, r0
//|.define CURSCORE, r11
//|.define IMGCURX, r8
//|.define IMGCURY, r9
//|.define IMGCURSTATE, r12
//|.define IMGCURINDEX, r15    
//|.define IMGLOCATIONREG, r6
//|.define TARGETLOCATIONREG, r2
    
// IMGCURSTATE 0 = ready to receive x, 1 = ready to receive y, 2 = ready to receive color 
// Note BESTSCORE is the return value, and r1 passes in the problem-specific initial score for the initial all-0 image.
    
void *softasm(int codes[NUMINSTRUCTIONS]) {
  dasm_State *state;
  dasm_init(&state, 1);
  dasm_setup(&state, actions);  

  //|  push r3
  //|  push r12  
  //|  push r13
  //|  push r14
  //|  push r15  
  //|  mov CYCLECOUNTREG,[CYCLECOUNTLOCATIONREG]
  //|  mov qword [IMGLOCATIONREG+CONSTLOC0],0
  //|  mov qword [IMGLOCATIONREG+CONSTLOC1],1
  //|  mov qword [IMGLOCATIONREG+CONSTLOC2],2
  //|  mov qword [IMGLOCATIONREG+CONSTLOC999],999
  //|  mov qword [IMGLOCATIONREG+CONSTLOCNEG999],-999
  //|  xor ACC,ACC
  //|  xor BAK,BAK
  //|  mov CURSCORE,r1
  //|  mov BESTSCORE,r1  
  //|  xor IMGCURSTATE,IMGCURSTATE
  //|  xor TMP,TMP
  dasm_put(Dst, 0, CONSTLOC0, CONSTLOC1, CONSTLOC2, CONSTLOC999, CONSTLOCNEG999);
#line 87 "softasm-tis100.dasc"

  dasm_growpc(&state, NUMINSTRUCTIONS+3);

  int nonnopflag = 0;
  for(int i=0;i<NUMINSTRUCTIONS;i++) {
    //|=> (i):
    dasm_put(Dst, 76,  (i));
#line 93 "softasm-tis100.dasc"
    int code = codes[i];
    int instr = code>>VBITS;
    int operand = code&VMASK;
    int origoperand = operand;
    int jumptargetoperand = ((double) origoperand)*((double) (((double) NUMINSTRUCTIONS)/((double) VVALS)));
    if(jumptargetoperand<0) {
      jumptargetoperand = 0;
    }
    if(jumptargetoperand>=NUMINSTRUCTIONS) {
      jumptargetoperand = NUMINSTRUCTIONS-1;
    }
    operand = operand + IMMEDIATECONSTANTOFFSET;

    if(instr==NOP) { // pseudoinstruction; generates no code and uses no cycles
    } else {
      nonnopflag = 1;
      // most instructions use 1 cycle
      //| sub CYCLECOUNTREG,1
      //| jle =>(NUMINSTRUCTIONS+2)
      dasm_put(Dst, 78, (NUMINSTRUCTIONS+2));
#line 112 "softasm-tis100.dasc"

      if(instr==JMP) {
        //| jmp =>(jumptargetoperand)
        dasm_put(Dst, 87, (jumptargetoperand));
#line 115 "softasm-tis100.dasc"
      } else if(instr==JEZ) {
        //| cmp ACC,0
        //| jz =>(jumptargetoperand)
        dasm_put(Dst, 91, (jumptargetoperand));
#line 118 "softasm-tis100.dasc"
      } else if(instr==JNZ) {
        //| cmp ACC,0
        //| jnz =>(jumptargetoperand)
        dasm_put(Dst, 100, (jumptargetoperand));
#line 121 "softasm-tis100.dasc"
      } else if(instr==JGZ) {
        //| cmp ACC,0
        //| jg =>(jumptargetoperand)
        dasm_put(Dst, 109, (jumptargetoperand));
#line 124 "softasm-tis100.dasc"
      } else if(instr==JLZ) {
        //| cmp ACC,0
        //| jl =>(jumptargetoperand)
        dasm_put(Dst, 118, (jumptargetoperand));
#line 127 "softasm-tis100.dasc"
      } else if(instr==SWP) {
        //| mov TMP,ACC
	//| mov ACC,BAK
	//| mov BAK,TMP
	dasm_put(Dst, 127);
#line 131 "softasm-tis100.dasc"
      } else if(instr==SAV) {	   
	//| mov BAK,ACC
	dasm_put(Dst, 139);
#line 133 "softasm-tis100.dasc"
      } else if(instr==NEG) {	   
	//| neg ACC
	dasm_put(Dst, 144);
#line 135 "softasm-tis100.dasc"
      } else if(instr==ADDC) {	   
        //| add ACC,operand
        dasm_put(Dst, 149, operand);
#line 137 "softasm-tis100.dasc"
        // saturate at +/- 999
	//| cmp ACC,999
	//| cmovg ACC,[IMGLOCATIONREG+CONSTLOC999]
	//| cmp ACC,-999
	//| cmovl ACC,[IMGLOCATIONREG+CONSTLOCNEG999]
	dasm_put(Dst, 154, CONSTLOC999, CONSTLOCNEG999);
#line 142 "softasm-tis100.dasc"
      } else if(instr==ADDA) {	   
        //| add ACC,ACC
        dasm_put(Dst, 184);
#line 144 "softasm-tis100.dasc"
        // saturate at +/- 999
	//| cmp ACC,999
	//| cmovg ACC,[IMGLOCATIONREG+CONSTLOC999]
	//| cmp ACC,-999
	//| cmovl ACC,[IMGLOCATIONREG+CONSTLOCNEG999]
	dasm_put(Dst, 154, CONSTLOC999, CONSTLOCNEG999);
#line 149 "softasm-tis100.dasc"
      } else if(instr==SUBC) {	   
        //| sub ACC,operand
        dasm_put(Dst, 189, operand);
#line 151 "softasm-tis100.dasc"
        // saturate at +/- 999
	//| cmp ACC,999
	//| cmovg ACC,[IMGLOCATIONREG+CONSTLOC999]
	//| cmp ACC,-999
	//| cmovl ACC,[IMGLOCATIONREG+CONSTLOCNEG999]
	dasm_put(Dst, 154, CONSTLOC999, CONSTLOCNEG999);
#line 156 "softasm-tis100.dasc"
      } else if(instr==SUBA) {	// zeroes ACC
        //| mov ACC,0
        dasm_put(Dst, 195);
#line 158 "softasm-tis100.dasc"
      } else if(instr==MOVCA) {	
        //| mov ACC,operand
        dasm_put(Dst, 203, operand);
#line 160 "softasm-tis100.dasc"
      } else if((instr==MOVAD)||(instr==MOVCD)) {
        if(instr==MOVAD) {
          //| mov TMP,ACC
          dasm_put(Dst, 208);
#line 163 "softasm-tis100.dasc"
        } else {
          //| mov TMP,operand
          dasm_put(Dst, 213, operand);
#line 165 "softasm-tis100.dasc"
        }
        // these instructions use an extra cycle
        //| sub CYCLECOUNTREG,1
        //| jle =>(NUMINSTRUCTIONS+2)
        dasm_put(Dst, 78, (NUMINSTRUCTIONS+2));
#line 169 "softasm-tis100.dasc"

        //| cmp TMP,0
	//| cmovl IMGCURSTATE,[IMGLOCATIONREG+CONSTLOC0]
	//| jl =>(i+1)
	dasm_put(Dst, 218, CONSTLOC0, (i+1));
#line 173 "softasm-tis100.dasc"

	//| cmp IMGCURSTATE,0
	//| cmove IMGCURX,TMP
        //| cmove IMGCURSTATE,[IMGLOCATIONREG+CONSTLOC1]
	//| je =>(i+1)
	dasm_put(Dst, 232, CONSTLOC1, (i+1));
#line 178 "softasm-tis100.dasc"

	//| cmp IMGCURSTATE,1
	//| cmove IMGCURY,TMP
	//| cmove IMGCURSTATE,[IMGLOCATIONREG+CONSTLOC2]
	//| je =>(i+1)
	dasm_put(Dst, 250, CONSTLOC2, (i+1));
#line 183 "softasm-tis100.dasc"

        // Otherwise CURSTATE is 2.
	// Remaining case is nonnegative and is color.  First check if coordinates are out of window, in which case we ignore.
	//| cmp IMGCURX,MAXX
        //| jae =>(i+1)
	//| cmp IMGCURY,MAXY
        //| jae =>(i+1)
        dasm_put(Dst, 268, MAXX, (i+1), MAXY, (i+1));
#line 190 "softasm-tis100.dasc"
	  
	//| cmp TMP,4
	//| cmovg TMP,[IMGLOCATIONREG+CONSTLOC0]
	dasm_put(Dst, 285, CONSTLOC0);
#line 193 "softasm-tis100.dasc"
	  
	//| mov IMGCURINDEX,IMGCURY
	//| shl IMGCURINDEX,XBITS
	//| or IMGCURINDEX,IMGCURX
	dasm_put(Dst, 296, XBITS);
#line 197 "softasm-tis100.dasc"

	//| add IMGCURX,1
	dasm_put(Dst, 307);
#line 199 "softasm-tis100.dasc"
	  
	//| cmp TMP,[IMGLOCATIONREG+IMGCURINDEX*8]
	//| jz =>(i+1)
	dasm_put(Dst, 312, (i+1));
#line 202 "softasm-tis100.dasc"
	  
	//| mov TMPB,0
	//| cmp TMP,[TARGETLOCATIONREG+IMGCURINDEX*8]
	//| cmove TMPB,[IMGLOCATIONREG+CONSTLOC1]
	//| add CURSCORE,TMPB
	//| mov TMPB,[IMGLOCATIONREG+IMGCURINDEX*8]
        //| mov [IMGLOCATIONREG+IMGCURINDEX*8],TMP
	//| mov TMP,0
	//| cmp TMPB,[TARGETLOCATIONREG+IMGCURINDEX*8]
	//| cmove TMP,[IMGLOCATIONREG+CONSTLOC1]
	//| sub CURSCORE,TMP
	dasm_put(Dst, 321, CONSTLOC1, CONSTLOC1);
#line 213 "softasm-tis100.dasc"
	  
	//| cmp CURSCORE,BESTSCORE
	//| jle =>(i+1)
        //| mov BESTSCORE,CURSCORE
	//| cmp BESTSCORE,TARGETSCORE
	//| je =>(NUMINSTRUCTIONS+1)
	dasm_put(Dst, 372, (i+1), TARGETSCORE, (NUMINSTRUCTIONS+1));
#line 219 "softasm-tis100.dasc"
      } else {
        ERROREXIT("FATAL ERROR: bad instr %d %d\n",instr,code);
  } } }

  if(nonnopflag) { // otherwise the jmp will loop infinitely 
    // last instruction: loop back to start
    //|=> (NUMINSTRUCTIONS):
    //| jmp =>(0)
    dasm_put(Dst, 390,  (NUMINSTRUCTIONS), (0));
#line 227 "softasm-tis100.dasc"
  
    // Success exit
    //|=> (NUMINSTRUCTIONS+1):
    //|  mov [CYCLECOUNTLOCATIONREG],CYCLECOUNTREG
    //|  pop r15
    //|  pop r14    
    //|  pop r13
    //|  pop r12
    //|  pop r3    
    //|  ret
    dasm_put(Dst, 395,  (NUMINSTRUCTIONS+1));
#line 237 "softasm-tis100.dasc"
  }

  // Time limit exceeded exit
  //|=> (NUMINSTRUCTIONS+2):
  //|  mov [CYCLECOUNTLOCATIONREG],CYCLECOUNTREG
  //|  pop r15
  //|  pop r14    
  //|  pop r13
  //|  pop r12
  //|  pop r3    
  //|  ret  
  dasm_put(Dst, 395,  (NUMINSTRUCTIONS+2));
#line 248 "softasm-tis100.dasc"

  size_t size;
  dasm_link(&state, &size);
  char *m = mmap(NULL, size + sizeof(size_t),PROT_READ | PROT_WRITE,MAP_ANON | MAP_PRIVATE, -1, 0);
  *(size_t*)m = size;
  void *f = m + sizeof(size_t);
  dasm_encode(&state, f);
  dasm_free(&state);
  mprotect(m, size, PROT_EXEC | PROT_READ);

  return(f);
}

long long int (*fptr)(long long int *o,long long int *p,long long int *q,long long int b);
int savecodes[NUMINSTRUCTIONS]; // for tracking bestcycles

void softasm_setup(int codes[NUMINSTRUCTIONS]) {
  fptr = softasm(codes);
  if(fptr==NULL) ERROREXIT("FATAL ERROR: fptr %d...\n",codes[0]);
  for(int i=0;i<NUMINSTRUCTIONS;i++) savecodes[i]=codes[i];
}

void softasm_cleanup() {
  void *m = (char *)fptr - sizeof(size_t);
  munmap(m, *(size_t*)m);
}	    
