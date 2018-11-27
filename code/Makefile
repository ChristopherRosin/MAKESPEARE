CC=gcc
LUAJITDIR=./luajit-2.0  # Configure with your location of LuaJIT, available from https://luajit.org/download.html

all: makespeare-x86-64 makespeare-tis100

makespeare-x86-64: search-dahc.c
	$(CC) -m64 -O3 -std=gnu99 -I$(LUAJITDIR) -I ./x86-64 -o makespeare-x86-64 search-dahc.c -lm

makespeare-tis100: search-dahc.c
	$(CC) -m64 -O3 -std=gnu99 -I$(LUAJITDIR) -I ./tis-100 -o makespeare-tis100 search-dahc.c -lm

