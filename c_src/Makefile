Run:
	@echo "Now compile a SWC"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -O4 $(LUA_FILE) main.cpp -emit-swc=snjdck.lua -o lua.swc -lAS3++

FLASCC:=/cygdrive/c/Crossbridge_1.0.1/sdk
BASE_CFLAGS:=-Werror -Wno-write-strings -Wno-trigraphs
LUA_FILE:=lapi.c lauxlib.c lbaselib.c lbitlib.c lcode.c lcorolib.c lctype.c ldblib.c ldebug.c ldo.c ldump.c lfunc.c lgc.c linit.c liolib.c llex.c lmathlib.c lmem.c loadlib.c lobject.c lopcodes.c loslib.c lparser.c lstate.c lstring.c lstrlib.c ltable.c ltablib.c ltm.c lundump.c lutf8lib.c lvm.c lzio.c
