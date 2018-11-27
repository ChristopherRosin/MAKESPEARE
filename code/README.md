# MAKESPEARE/code

Copyright 2018 Christopher D. Rosin

System Requirements: the experiments in the paper were run under Ubuntu Linux version 14.04, compiled with gcc version 5.4.0.  Other versions may work as well.  The software requires a 64-bit x86-64 CPU.  

To build the code, you will need to download LuaJIT (which contains DynASM) from:
<https://luajit.org/download.html>
Version LuaJIT-2.1.0-beta3 works.

Configure LUAJITDIR in the Makefile with the location of your LuaJIT directory.

Run:
```
make
```
in this code directory.  This should build makespeare-x86-64 and makespeare-tis100.

Run with no arguments to get a summary of command-line parameters.  You can use any random seeds you want, but to reproduce the experiments in the MAKESPEARE AAAI paper use:
* Seeds 50100-50199 for batches of 100 runs (with I=75k and I=2M)
* Seeds 50101-50130 for batches of 30 runs (with I=100M)
* Seeds 50101-50110, with numruns 500 each, for the TIS-100 Image Test Pattern 2 results.
For example, with Delayed Acceptance, Integer Sqrt succeeds at I=2M; to reproduce these results obtain the integer-sqrt.tsv file from ../benchmarks/ and run:
```
makespeare-x86-64 501xx integer-sqrt.tsv 2.0 0.0 1.0 0.0 2000000 9
```
for each xx=00 to 99.  

For the x86-64 benchmarks, the TSV input format has one example per line, with tab-separated fields.  See the header of the ../benchmarks/ TSV files for a listing of the fields.  The first field is 0 for training set, 1 for test set.  The input memory array is a white-space separated list of integers contained in a single field, as is the output memory array.

The input file format for makespeare-tis100 is a sequence of 540 whitespace-separated integers in {0,1,2,3}.  

The output log reports the end of each Delayed Acceptance period, as well as each program that achieves a new best result during the run.  The programs are listed in two columns: the first is a readable assembly language format, and the second is the internal codes used to represent the program.  For TIS-100, the first column's program listing can be pasted into the TIS-100 game available from <http://www.zachtronics.com/> - remove the NOPs to get full credit for short programs.  For x86-64, the first column's program listing omits redundant/unnecessary ARGs.

code/x86-64 and code/tis-100 each contain softasm.h which has been built from softasm.dasc using DynASM.  If you need to change softasm.dasc you'll need to install Lua version 5.1 to rebuild softasm.h as follows:
```
lua5.1 LUAJITDIR/dynasm/dynasm.lua softasm.dasc > softasm.h
```
where LUAJITDIR is the location of your LuaJIT directory as configured in the Makefile.

While most parameters are on the command line, the TIS-100 version has a compile-time parameter NUMIMMEDIATECONSTANTS in softasm.dasc - this can be set to the values for P listed in Table 2 of the MAKESPEARE AAAI paper.

While most recent Intel or AMD CPUs should be able to run the code, the experiments were run on Intel i7-6700 CPUs (Skylake), and it is possible this detail matters if you need to exactly reproduce the MAKESPEARE AAAI paper's results.  Synthesized programs can leverage the semantics of the underlying hardware, and could possible use undefined/undocumented features (e.g. the values of flags after IMUL) that could change between versions of the CPU.  

It is recommended that you run MAKESPEARE within a protected environment, e.g. using firejail <https://firejail.wordpress.com/>.  MAKESPEARE compiles and runs many random programs, and if there's any bug causing failure of MAKESPEARE's internal protections it could be possible for a generated program to trigger system calls with unwanted effects.











