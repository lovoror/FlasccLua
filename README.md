# FlasccLua
Embed lua runtime in as3.

The embed lua version is 5.3.1, it's source files are all unmodified.
I just add two files in the 'c_src' directory: 'Makefile' and 'main.cpp'.

There are six functions exported to as3:

public function lua_newState():int

public function lua_close(L:int):void

public function lua_doString(L:int, str:String):void

public function lua_invoke(L:int, name:String, argList:Array):*

public function lua_getVar(L:int, name:String):*

public function lua_setVar(L:int, name:String, value:Object):void

