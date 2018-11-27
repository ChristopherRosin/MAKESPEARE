/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x64 version 1.4.0
** DO NOT EDIT! The original file is in "softasm-x86.dasc".
*/

#line 1 "softasm-x86.dasc"
// Copyright 2018 Christopher D. Rosin
// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdint.h>
#include <sys/mman.h>

#define NUMINSTRUCTIONS 32 // number of slots
#define NUMIMMEDIATECONSTANTS 4
#define NUMREGISTERS 6
#define VBITS 4            // "V" for operands
#define VMASK ((0x1<<VBITS)-1)
#define OBITS 4            // "O" for opcodes
#define OVALS 14           // must be <= 2^OBITS.  And must match the number of opcodes defined and handled below.
#define OLMASK ((0x1<<OBITS)-1)
#define OMASK (OLMASK<<VBITS)
#define MAXCODES (0x1<<(OBITS+VBITS))
#define ARCHREG 16

int IVALS; // must call setupdata to set
int VVALS; // must call setupdata to set

enum opcodes {MOV,ADD,SUB,IMUL,SHR,SHL,CMP,TEST,INC,ARG,JMP,JZ,JNZ,JG};
const char* oname[] = {"MOV","ADD","SUB","IMUL","SHR","SHL","CMP","TEST","INC","ARG","JMP","JZ","JNZ","JG"};
char istr[MAXCODES][14];

#define N ARG

#define Dst &state

const int immediateconstants[NUMIMMEDIATECONSTANTS] = {0,1,2,3};
const int registers[NUMREGISTERS] = {7,6,2,0,8,9};  // calling-convention registers, except r1 is needed elsewhere and so replaced by r0 (which is also return value)

//|.arch x64
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 36 "softasm-x86.dasc"
//|.actionlist actions
static const unsigned char actions[554] = {
  88,65,90,65,91,65,83,65,82,80,65,86,65,87,73,137,252,246,73,131,198,1,77,
  139,59,72,137,200,72,49,201,255,249,255,156,73,131,252,239,1,15,132,245,157,
  252,233,245,255,73,131,252,239,1,15,143,245,252,233,245,255,15,133,245,255,
  15,132,245,255,15,142,245,255,15,143,245,255,72,131,192,240,35,1,255,156,
  76,57,252,240,240,35,15,131,245,157,255,76,57,252,240,240,35,15,131,245,255,
  73,131,4,194,240,68,1,255,72,199,192,240,35,237,255,72,129,192,240,35,239,
  255,72,129,232,240,35,239,255,72,105,192,240,131,240,35,239,255,72,193,232,
  240,35,235,255,72,193,224,240,35,235,255,72,129,252,248,240,35,239,255,72,
  252,247,192,240,35,237,255,72,137,192,240,131,240,35,255,72,1,192,240,131,
  240,35,255,72,41,192,240,131,240,35,255,72,15,175,192,240,132,240,36,255,
  72,137,193,240,131,72,211,232,240,35,255,72,137,193,240,131,72,211,224,240,
  35,255,72,57,192,240,131,240,35,255,72,133,192,240,131,240,35,255,73,139,
  4,240,131,194,240,68,255,73,3,4,240,131,194,240,68,255,73,43,4,240,131,194,
  240,68,255,73,15,175,4,240,132,194,240,69,255,73,139,12,194,240,68,72,211,
  232,240,35,255,73,139,12,194,240,68,72,211,224,240,35,255,73,59,4,240,131,
  194,240,68,255,73,133,4,240,131,194,240,68,255,73,199,4,194,240,68,237,255,
  73,129,4,253,194,240,68,239,255,73,129,44,253,194,240,68,239,255,72,199,193,
  237,73,15,175,12,194,240,69,73,137,12,194,240,68,255,73,193,44,194,240,68,
  235,255,73,193,36,194,240,68,235,255,73,129,60,253,194,240,68,239,255,73,
  252,247,4,194,240,68,237,255,73,137,4,240,131,194,240,68,255,73,1,4,240,131,
  194,240,68,255,73,41,4,240,131,194,240,68,255,72,137,193,240,131,73,15,175,
  12,194,240,69,73,137,12,194,240,68,255,72,137,193,240,131,73,211,44,194,240,
  68,255,72,137,193,240,131,73,211,36,194,240,68,255,73,57,4,240,131,194,240,
  68,255,73,139,12,194,240,68,255,73,1,12,194,240,68,255,73,41,12,194,240,68,
  255,73,57,12,194,240,68,255,73,133,12,194,240,68,255,249,77,137,59,65,95,
  65,94,195,255,249,157,73,252,247,223,77,137,59,65,95,65,94,195,255,249,73,
  252,247,223,77,137,59,65,95,65,94,195,255
};

#line 37 "softasm-x86.dasc"
//|.define LOOPCOUNTREG, r15
//|.define LOOPCOUNTLOCATIONREG, r11
//|.define MEMBASEREG, r10
//|.define MEMBOUNDREG, r14

//|.macro loopcountboundcheck
  //||	if(flagsunneeded[jumptargetoperand]==0) {
	  //| pushf
	  //| sub LOOPCOUNTREG,1
	  //| jz =>(NUMINSTRUCTIONS+1)
	  //| popf
	  //| jmp =>(jumptargetoperand)
  //||	} else {
	  //| sub LOOPCOUNTREG,1
	  //| jg =>(jumptargetoperand)
	  //| jmp =>(NUMINSTRUCTIONS+2)
  //||    }
//|.endmacro

//|.macro memboundcheck, arg1
  //||	  if(boundschecked[arg1]==0) {
  //||        if(flagsunneeded[i]==0) {
	      //| pushf
              //| cmp Rq(arg1),MEMBOUNDREG
              //| jae =>(NUMINSTRUCTIONS+1)
	      //| popf
  //||	    } else {
              //| cmp Rq(arg1),MEMBOUNDREG
              //| jae =>(NUMINSTRUCTIONS+2)
  //||	    }
  //||	    boundschecked[arg1] = 1;
  //||	  }
//|.endmacro

//|.macro finishup
  //| mov [LOOPCOUNTLOCATIONREG],LOOPCOUNTREG
  //| pop r15
  //| pop r14
  //| ret
//|.endmacro


void *softasm(int codes[NUMINSTRUCTIONS]) {
  dasm_State *state;
  dasm_init(&state, 1);
  dasm_setup(&state, actions);  

  int destreg = 0;   // default ARG. 
  int desttype = 0;  // 0 == reg, 1 == mem.

  // temporarily pop, then restore stack
  //|  pop r0  
  //|  pop MEMBASEREG
  //|  pop LOOPCOUNTLOCATIONREG
  //|  push LOOPCOUNTLOCATIONREG
  //|  push MEMBASEREG
  //|  push r0
  //|  push r14
  //|  push r15
  //|  mov MEMBOUNDREG,r6
  //|  add MEMBOUNDREG,1
  //|  mov LOOPCOUNTREG,[LOOPCOUNTLOCATIONREG]
  //|  mov r0,r1
  //|  xor r1,r1
  dasm_put(Dst, 0);
#line 101 "softasm-x86.dasc"
  // Final xor clears flags to a known state.

  dasm_growpc(&state, NUMINSTRUCTIONS+3);

  int boundschecked[ARCHREG];
  for(int i=0;i<NUMREGISTERS;i++) boundschecked[registers[i]] = 0;
  int jumptarget[NUMINSTRUCTIONS];
  for(int i=0;i<NUMINSTRUCTIONS;i++) jumptarget[i]=0;
  for(int i=0;i<NUMINSTRUCTIONS;i++) {
    int instr = codes[i]>>VBITS;
    if((instr>=JMP)&&(instr<=JG)) {
      jumptarget[((codes[i])&VMASK)*(NUMINSTRUCTIONS/VVALS)] = 1;
  } }
  int flagsunneeded[NUMINSTRUCTIONS];
  int instr = codes[NUMINSTRUCTIONS-1]>>VBITS;
  if((instr>=JMP)&&(instr<=JG)) flagsunneeded[NUMINSTRUCTIONS-1]=0;
    else flagsunneeded[NUMINSTRUCTIONS-1]=1;  
  for(int i=NUMINSTRUCTIONS-2;i>=0;i--) {
    int instr = codes[i]>>VBITS;
    if((instr>=JMP)&&(instr<=JG)) flagsunneeded[i] = 0;
      else if(flagsunneeded[i+1]) flagsunneeded[i] = 1;
      else if((instr==MOV)||(instr==SHR)||(instr==SHL)||(instr==IMUL)||(instr==ARG)||((instr==INC)&&((codes[i]&VMASK)<NUMIMMEDIATECONSTANTS))) flagsunneeded[i] = 0;
      else flagsunneeded[i] = 1;
  }

  for(int i=0;i<NUMINSTRUCTIONS;i++) {
    //|=> (i):
    dasm_put(Dst, 32,  (i));
#line 128 "softasm-x86.dasc"
    int code = codes[i];
    int instr = code>>VBITS;
    int operand = code&VMASK;
    int origoperand = operand;
    int jumptargetoperand = operand*(NUMINSTRUCTIONS/VVALS);

    if(jumptarget[i]) {
      for(int j=0;j<NUMREGISTERS;j++) boundschecked[registers[j]] = 0;
    }

    if(instr==ARG) { // pseudoinstruction; generates no code
      if(origoperand>=NUMIMMEDIATECONSTANTS) { // otherwise ignore, with no effect
	if(operand-NUMIMMEDIATECONSTANTS<NUMREGISTERS) {
	  destreg = registers[operand-NUMIMMEDIATECONSTANTS];
	  desttype = 0;
	} else {
	  destreg = registers[operand-NUMIMMEDIATECONSTANTS-NUMREGISTERS];
	  desttype = 1;
      } }
    } else if(instr==JMP) {
      if(jumptargetoperand<=i) { // backjump
        //| loopcountboundcheck
        	if(flagsunneeded[jumptargetoperand]==0) {
        dasm_put(Dst, 34, (NUMINSTRUCTIONS+1), (jumptargetoperand));
        	} else {
        dasm_put(Dst, 48, (jumptargetoperand), (NUMINSTRUCTIONS+2));
            }
#line 150 "softasm-x86.dasc"
       } else {
        //| jmp =>(jumptargetoperand)
        dasm_put(Dst, 44, (jumptargetoperand));
#line 152 "softasm-x86.dasc"
       }
    } else if(instr==JZ) {
      if(jumptargetoperand<=i) { // backjump
	//| jnz =>(i+1)
	//| loopcountboundcheck
	dasm_put(Dst, 60, (i+1));
		if(flagsunneeded[jumptargetoperand]==0) {
	dasm_put(Dst, 34, (NUMINSTRUCTIONS+1), (jumptargetoperand));
		} else {
	dasm_put(Dst, 48, (jumptargetoperand), (NUMINSTRUCTIONS+2));
	    }
#line 157 "softasm-x86.dasc"
      } else {
        //| jz =>(jumptargetoperand)
        dasm_put(Dst, 64, (jumptargetoperand));
#line 159 "softasm-x86.dasc"
      }
    } else if(instr==JNZ) {
      if(jumptargetoperand<=i) { // backjump
	//| jz =>(i+1)
	//| loopcountboundcheck
	dasm_put(Dst, 64, (i+1));
		if(flagsunneeded[jumptargetoperand]==0) {
	dasm_put(Dst, 34, (NUMINSTRUCTIONS+1), (jumptargetoperand));
		} else {
	dasm_put(Dst, 48, (jumptargetoperand), (NUMINSTRUCTIONS+2));
	    }
#line 164 "softasm-x86.dasc"
      } else {
        //| jnz =>(jumptargetoperand)
        dasm_put(Dst, 60, (jumptargetoperand));
#line 166 "softasm-x86.dasc"
      }
    } else if(instr==JG) {
      if(jumptargetoperand<=i) { // backjump
	//| jle =>(i+1)
	//| loopcountboundcheck
	dasm_put(Dst, 68, (i+1));
		if(flagsunneeded[jumptargetoperand]==0) {
	dasm_put(Dst, 34, (NUMINSTRUCTIONS+1), (jumptargetoperand));
		} else {
	dasm_put(Dst, 48, (jumptargetoperand), (NUMINSTRUCTIONS+2));
	    }
#line 171 "softasm-x86.dasc"
      } else {
        //| jg =>(jumptargetoperand)
        dasm_put(Dst, 72, (jumptargetoperand));
#line 173 "softasm-x86.dasc"
      }
    } else { 
      int constflag = 0;
      int memflag = 0;
      if(operand<NUMIMMEDIATECONSTANTS) {
	constflag = 1;
	operand = immediateconstants[operand];
      } else if(operand-NUMIMMEDIATECONSTANTS<NUMREGISTERS) {
	operand = registers[operand-NUMIMMEDIATECONSTANTS];
      } else {
	operand = registers[operand-NUMIMMEDIATECONSTANTS-NUMREGISTERS];
	memflag = 1;
      }
      if(instr==INC) { // Special case; INC uses operand as destination.
	if(constflag==0) { // const destination is ignored; generate no code in that case.
	  if(memflag==0) {
  	    //| add Rq(operand),1
  	    dasm_put(Dst, 76, (operand));
#line 190 "softasm-x86.dasc"
	    boundschecked[operand] = 0;
          } else {
	    //| memboundcheck operand
	    	  if(boundschecked[operand]==0) {
	            if(flagsunneeded[i]==0) {
	    dasm_put(Dst, 83, (operand), (NUMINSTRUCTIONS+1));
	    	    } else {
	    dasm_put(Dst, 95, (operand), (NUMINSTRUCTIONS+2));
	    	    }
	    	    boundschecked[operand] = 1;
	    	  }
#line 193 "softasm-x86.dasc"
            //| add qword [MEMBASEREG + Rq(operand)*8],1
            dasm_put(Dst, 105, (operand));
#line 194 "softasm-x86.dasc"
        } }
      } else if(desttype==0) {  // reg destination 
	if(constflag==1) { 
  	  if(instr==MOV) {
	    //| mov Rq(destreg),operand
	    dasm_put(Dst, 113, (destreg), operand);
#line 199 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;
          } else if(instr==ADD) {
  	    //| add Rq(destreg),operand
  	    dasm_put(Dst, 120, (destreg), operand);
#line 202 "softasm-x86.dasc"
	    if(operand!=0) boundschecked[destreg] = 0;
	  } else if(instr==SUB) {
	    //| sub Rq(destreg),operand
	    dasm_put(Dst, 127, (destreg), operand);
#line 205 "softasm-x86.dasc"
	    if(operand!=0) boundschecked[destreg] = 0;	    
	  } else if(instr==IMUL) {
	    //| imul Rq(destreg),operand
	    dasm_put(Dst, 134, (destreg), (destreg), operand);
#line 208 "softasm-x86.dasc"
	    if(operand!=1) boundschecked[destreg] = 0;	    
	  } else if(instr==SHR) {
	    //| shr Rq(destreg),operand
	    dasm_put(Dst, 143, (destreg), operand);
#line 211 "softasm-x86.dasc"
	    if(operand!=0) boundschecked[destreg] = 0;	    
	  } else if(instr==SHL) {
	    //| shl Rq(destreg),operand
	    dasm_put(Dst, 150, (destreg), operand);
#line 214 "softasm-x86.dasc"
	    if(operand!=0) boundschecked[destreg] = 0;	    	    
          } else if(instr==CMP) {
	    //| cmp Rq(destreg),operand
	    dasm_put(Dst, 157, (destreg), operand);
#line 217 "softasm-x86.dasc"
	  } else if(instr==TEST) {
	    //| test Rq(destreg),operand
	    dasm_put(Dst, 165, (destreg), operand);
#line 219 "softasm-x86.dasc"
	  }
	} else if(memflag==0) {
	  if(instr==MOV) {
	    //| mov Rq(destreg),Rq(operand)
	    dasm_put(Dst, 173, (operand), (destreg));
#line 223 "softasm-x86.dasc"
	    boundschecked[destreg] = boundschecked[operand];
	  } else if(instr==ADD) {
 	    //| add Rq(destreg),Rq(operand)
 	    dasm_put(Dst, 181, (operand), (destreg));
#line 226 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    
	  } else if(instr==SUB) {
	    //| sub Rq(destreg),Rq(operand)
	    dasm_put(Dst, 189, (operand), (destreg));
#line 229 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    
	  } else if(instr==IMUL) {
	    //| imul Rq(destreg),Rq(operand)
	    dasm_put(Dst, 197, (destreg), (operand));
#line 232 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    
	  } else if(instr==SHR) {
	    //| mov r1,Rq(operand)
	    //| shr Rq(destreg),cl
	    dasm_put(Dst, 206, (operand), (destreg));
#line 236 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    
	  } else if(instr==SHL) {
	    //| mov r1,Rq(operand)
	    //| shl Rq(destreg),cl
	    dasm_put(Dst, 217, (operand), (destreg));
#line 240 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    
	  } else if(instr==CMP) {
	    //| cmp Rq(destreg),Rq(operand)
	    dasm_put(Dst, 228, (operand), (destreg));
#line 243 "softasm-x86.dasc"
	  } else if(instr==TEST) {
	    //| test Rq(destreg),Rq(operand)
	    dasm_put(Dst, 236, (operand), (destreg));
#line 245 "softasm-x86.dasc"
	  }
        } else { // memflag==1
	  //| memboundcheck operand
	  	  if(boundschecked[operand]==0) {
	          if(flagsunneeded[i]==0) {
	  dasm_put(Dst, 83, (operand), (NUMINSTRUCTIONS+1));
	  	    } else {
	  dasm_put(Dst, 95, (operand), (NUMINSTRUCTIONS+2));
	  	    }
	  	    boundschecked[operand] = 1;
	  	  }
#line 248 "softasm-x86.dasc"
	  if(instr==MOV) {
	    //| mov Rq(destreg),[MEMBASEREG + Rq(operand)*8]
	    dasm_put(Dst, 244, (destreg), (operand));
#line 250 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    	    
	  } else if(instr==ADD) {
	    //| add Rq(destreg),[MEMBASEREG + Rq(operand)*8]
	    dasm_put(Dst, 253, (destreg), (operand));
#line 253 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    	    
	  } else if(instr==SUB) {
	    //| sub Rq(destreg),[MEMBASEREG + Rq(operand)*8]
	    dasm_put(Dst, 262, (destreg), (operand));
#line 256 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    	    
	  } else if(instr==IMUL) {
	    //| imul Rq(destreg),[MEMBASEREG + Rq(operand)*8]
	    dasm_put(Dst, 271, (destreg), (operand));
#line 259 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    	    
	  } else if(instr==SHR) {
	    //| mov r1,[MEMBASEREG + Rq(operand)*8]
            //| shr Rq(destreg),cl
            dasm_put(Dst, 281, (operand), (destreg));
#line 263 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    	    
	  } else if(instr==SHL) {
	    //| mov r1,[MEMBASEREG + Rq(operand)*8]
            //| shl Rq(destreg),cl
            dasm_put(Dst, 293, (operand), (destreg));
#line 267 "softasm-x86.dasc"
	    boundschecked[destreg] = 0;	    	    
	  } else if(instr==CMP) {
	    //| cmp Rq(destreg),[MEMBASEREG + Rq(operand)*8]
	    dasm_put(Dst, 305, (destreg), (operand));
#line 270 "softasm-x86.dasc"
	  } else if(instr==TEST) {
	    //| test Rq(destreg),[MEMBASEREG + Rq(operand)*8]
	    dasm_put(Dst, 314, (destreg), (operand));
#line 272 "softasm-x86.dasc"
	} }
      } else { // desttype==1
        //| memboundcheck destreg
        	  if(boundschecked[destreg]==0) {
                if(flagsunneeded[i]==0) {
        dasm_put(Dst, 83, (destreg), (NUMINSTRUCTIONS+1));
        	    } else {
        dasm_put(Dst, 95, (destreg), (NUMINSTRUCTIONS+2));
        	    }
        	    boundschecked[destreg] = 1;
        	  }
#line 275 "softasm-x86.dasc"
	if(constflag==1) { 
  	  if(instr==MOV) {
	    //| mov qword [MEMBASEREG + Rq(destreg)*8],operand
	    dasm_put(Dst, 323, (destreg), operand);
#line 278 "softasm-x86.dasc"
          } else if(instr==ADD) {
  	    //| add qword [MEMBASEREG + Rq(destreg)*8],operand
  	    dasm_put(Dst, 331, (destreg), operand);
#line 280 "softasm-x86.dasc"
	  } else if(instr==SUB) {
	    //| sub qword [MEMBASEREG + Rq(destreg)*8],operand
	    dasm_put(Dst, 340, (destreg), operand);
#line 282 "softasm-x86.dasc"
	  } else if(instr==IMUL) {
	    //| mov r1,operand
	    //| imul r1,[MEMBASEREG + Rq(destreg)*8]
	    //| mov qword [MEMBASEREG + Rq(destreg)*8],r1
	    dasm_put(Dst, 349, operand, (destreg), (destreg));
#line 286 "softasm-x86.dasc"
	  } else if(instr==SHR) {
	    //| shr qword [MEMBASEREG + Rq(destreg)*8],operand
	    dasm_put(Dst, 367, (destreg), operand);
#line 288 "softasm-x86.dasc"
	  } else if(instr==SHL) {
	    //| shl qword [MEMBASEREG + Rq(destreg)*8],operand
	    dasm_put(Dst, 375, (destreg), operand);
#line 290 "softasm-x86.dasc"
          } else if(instr==CMP) {
	    //| cmp qword [MEMBASEREG + Rq(destreg)*8],operand
	    dasm_put(Dst, 383, (destreg), operand);
#line 292 "softasm-x86.dasc"
	  } else if(instr==TEST) {
	    //| test qword [MEMBASEREG + Rq(destreg)*8],operand
	    dasm_put(Dst, 392, (destreg), operand);
#line 294 "softasm-x86.dasc"
	  }
	} else if(memflag==0) {
	  if(instr==MOV) {
	    //| mov qword [MEMBASEREG + Rq(destreg)*8],Rq(operand)
	    dasm_put(Dst, 401, (operand), (destreg));
#line 298 "softasm-x86.dasc"
	  } else if(instr==ADD) {
 	    //| add qword [MEMBASEREG + Rq(destreg)*8],Rq(operand)
 	    dasm_put(Dst, 410, (operand), (destreg));
#line 300 "softasm-x86.dasc"
	  } else if(instr==SUB) {
	    //| sub qword [MEMBASEREG + Rq(destreg)*8],Rq(operand)
	    dasm_put(Dst, 419, (operand), (destreg));
#line 302 "softasm-x86.dasc"
	  } else if(instr==IMUL) {
	    //| mov r1,Rq(operand)
	    //| imul r1,[MEMBASEREG + Rq(destreg)*8]
	    //| mov qword [MEMBASEREG + Rq(destreg)*8],r1	    
	    dasm_put(Dst, 428, (operand), (destreg), (destreg));
#line 306 "softasm-x86.dasc"
	  } else if(instr==SHR) {
	    //| mov r1,Rq(operand)
	    //| shr qword [MEMBASEREG + Rq(destreg)*8],cl
	    dasm_put(Dst, 447, (operand), (destreg));
#line 309 "softasm-x86.dasc"
	  } else if(instr==SHL) {
	    //| mov r1,Rq(operand)
	    //| shl qword [MEMBASEREG + Rq(destreg)*8],cl
	    dasm_put(Dst, 459, (operand), (destreg));
#line 312 "softasm-x86.dasc"
	  } else if(instr==CMP) {
	    //| cmp qword [MEMBASEREG + Rq(destreg)*8],Rq(operand)
	    dasm_put(Dst, 471, (operand), (destreg));
#line 314 "softasm-x86.dasc"
	  } else if(instr==TEST) {
	    //| test qword [MEMBASEREG + Rq(destreg)*8],Rq(operand)
	    dasm_put(Dst, 314, (operand), (destreg));
#line 316 "softasm-x86.dasc"
	  }
        } else { // memflag==1
	  //| memboundcheck operand
	  	  if(boundschecked[operand]==0) {
	          if(flagsunneeded[i]==0) {
	  dasm_put(Dst, 83, (operand), (NUMINSTRUCTIONS+1));
	  	    } else {
	  dasm_put(Dst, 95, (operand), (NUMINSTRUCTIONS+2));
	  	    }
	  	    boundschecked[operand] = 1;
	  	  }
#line 319 "softasm-x86.dasc"
	  //| mov r1,[MEMBASEREG + Rq(operand)*8]
	  dasm_put(Dst, 480, (operand));
#line 320 "softasm-x86.dasc"
	  if(instr==MOV) {
	    //| mov qword [MEMBASEREG + Rq(destreg)*8],r1
	    dasm_put(Dst, 360, (destreg));
#line 322 "softasm-x86.dasc"
	  } else if(instr==ADD) {
	    //| add qword [MEMBASEREG + Rq(destreg)*8],r1
	    dasm_put(Dst, 487, (destreg));
#line 324 "softasm-x86.dasc"
	  } else if(instr==SUB) {
	    //| sub qword [MEMBASEREG + Rq(destreg)*8],r1
	    dasm_put(Dst, 494, (destreg));
#line 326 "softasm-x86.dasc"
	  } else if(instr==IMUL) {
	    //| imul r1,[MEMBASEREG + Rq(destreg)*8]
	    //| mov qword [MEMBASEREG + Rq(destreg)*8],r1	    	    
	    dasm_put(Dst, 353, (destreg), (destreg));
#line 329 "softasm-x86.dasc"
	  } else if(instr==SHR) {
            //| shr qword [MEMBASEREG + Rq(destreg)*8],cl
            dasm_put(Dst, 452, (destreg));
#line 331 "softasm-x86.dasc"
	  } else if(instr==SHL) {
            //| shl qword [MEMBASEREG + Rq(destreg)*8],cl
            dasm_put(Dst, 464, (destreg));
#line 333 "softasm-x86.dasc"
	  } else if(instr==CMP) {
	    //| cmp qword [MEMBASEREG + Rq(destreg)*8],r1
	    dasm_put(Dst, 501, (destreg));
#line 335 "softasm-x86.dasc"
	  } else if(instr==TEST) {
	    //| test qword [MEMBASEREG + Rq(destreg)*8],r1
	    dasm_put(Dst, 508, (destreg));
#line 337 "softasm-x86.dasc"
  } } } } }

  //|=> (NUMINSTRUCTIONS):
  //|  finishup
  dasm_put(Dst, 515,  (NUMINSTRUCTIONS));
#line 341 "softasm-x86.dasc"

  // bounds violation; mark it with neg loopcount (though if it was loopcount bound violation, then loopcount is zero anyway).  Need to popf the pushed condition flags.
  //|=> (NUMINSTRUCTIONS+1):
  //|  popf
  //|  neg LOOPCOUNTREG
  //|  finishup
  dasm_put(Dst, 525,  (NUMINSTRUCTIONS+1));
#line 347 "softasm-x86.dasc"

  // bounds violation, but no popf needed.
  //|=> (NUMINSTRUCTIONS+2):
  //|  neg LOOPCOUNTREG
  //|  finishup
  dasm_put(Dst, 540,  (NUMINSTRUCTIONS+2));
#line 352 "softasm-x86.dasc"

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

long long int (*fptr)(long long int r0,long long int r1,long long int r2,long long int r3,long long int r4,long long int r5,long long int *p,long long int *q);

void softasm_setup(int codes[NUMINSTRUCTIONS]) {
  fptr = softasm(codes);
  if(fptr==NULL) ERROREXIT("FATAL ERROR: fptr %d...\n",codes[0]);
}

void softasm_cleanup() {
  void *m = (char *)fptr - sizeof(size_t);
  munmap(m, *(size_t*)m);
}	    
