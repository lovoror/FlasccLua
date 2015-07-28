#include <stdlib.h>
#include <AS3/AS3.h>
#include <AS3/AS3++.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

using namespace AS3::local;
using namespace AS3::local::internal;

#define new_Error(msg) construct(getlex(new_String("Object")),msg)
#define as3 "__as3__"

#define EXPORT_AS3(name,sign) void name()__attribute__((used,annotate(sign),annotate("as3package:snjdck.lua")));

int main(){
	AS3_GoAsync();
}

var lua_tovar(lua_State* L, int index)
{
	index = lua_absindex(L, index);
	var result;
	int argType = lua_type(L, index);
	switch(argType)
	{
		case LUA_TSTRING:
			result = new_String(lua_tostring(L, index));
			break;
		case LUA_TNUMBER:
			if(lua_isinteger(L, index)){
				result = new_int(lua_tointeger(L, index));
			}else{
				result = new_Number(lua_tonumber(L, index));
			}
			break;
		case LUA_TBOOLEAN:
			result = new_Boolean(lua_toboolean(L, index));
			break;
		case LUA_TNIL:
			result = _null;
			break;
		case LUA_TUSERDATA:
			result = *(var*)lua_touserdata(L, index);
			break;
		case LUA_TTHREAD:
			result = new_int((int)lua_tothread(L, index));
			break;
		case LUA_TTABLE:
			result = new_Array(0, 0);
			lua_pushnil(L);
			while(lua_next(L, index)){
				var key = lua_isinteger(L, -2) ? new_int(lua_tointeger(L, -2) - 1) : lua_tovar(L, -2);
				setproperty(result, key, lua_tovar(L, -1));
				lua_pop(L, 1);
			}
			break;
		default:
			result = new_String(lua_typename(L, argType));
	}
	return result;
}

void lua_pushvar(lua_State* L, var value)
{
	if(equals(value, _null) || equals(value, _undefined)){
		lua_pushnil(L);
		return;
	}

	var varType = _typeof(value);

	if(equals(varType, new_String("number"))){
		lua_pushnumber(L, double_valueOf(value));
	}else if(equals(varType, new_String("string"))){
		char* str = utf8_toString(value);
		lua_pushstring(L, str);
		free(str);
	}else if(equals(varType, new_String("boolean"))){
		lua_pushboolean(L, bool_valueOf(value));
	}else if(equals(varType, new_String("xml"))){
		lua_pushnil(L);
	}else{
		var* func = (var*)lua_newuserdata(L, sizeof(var));
		*func = value;
		luaL_setmetatable(L, as3);
	}
}

int lua_pusharray(lua_State* L, var list)
{
	int count = int_valueOf(getproperty(list, new_String("length")));
	for(int i=0; i<count; ++i){
		lua_pushvar(L, getproperty(list, new_int(i)));
	}
	return count;
}

EXPORT_AS3(on_lua_newState,		"as3sig:public function lua_newState():int")
EXPORT_AS3(on_lua_close,		"as3sig:public function lua_close(L:int):void")
EXPORT_AS3(on_lua_doString,		"as3sig:public function lua_doString(L:int, str:String):void")
EXPORT_AS3(on_lua_invoke,		"as3sig:public function lua_invoke(L:int, name:String, argList:Array):*")
EXPORT_AS3(on_lua_getVar,		"as3sig:public function lua_getVar(L:int, name:String):*")
EXPORT_AS3(on_lua_setVar,		"as3sig:public function lua_setVar(L:int, name:String, value:Object):void")

int __index(lua_State* L)
{
	var target = lua_tovar(L, 1);
	var key = lua_tovar(L, 2);
	var value = getproperty(target, key);
	lua_pushvar(L, value);
	return 1;
}

int __newindex(lua_State* L)
{
	var target = lua_tovar(L, 1);
	var key = lua_tovar(L, 2);
	var value = lua_tovar(L, 3);
	setproperty(target, key, value);
	return 0;
}

int __eq(lua_State* L)
{
	var a = lua_tovar(L, 1);
	var b = lua_tovar(L, 2);
	lua_pushboolean(L, equals(a, b));
	return 1;
}

int __call(lua_State* L)
{
	var lambda = lua_tovar(L, 1);
	int argCount = lua_gettop(L) - 1;
	
	if(argCount <= 0){
		lua_pushvar(L, call(lambda, _null, 0, 0));
		return 1;
	}

	var argList = new_Array(argCount, 0);
	for(int i=0; i<argCount; ++i){
		setproperty(argList, new_int(i), lua_tovar(L, i + 2));
	}
	lua_pushvar(L, call_v(lambda, _null, argList));

	return 1;
}

const luaL_Reg methods[] = {
	{"__index",__index},
	{"__newindex",__newindex},
	{"__call",__call},
	{"__eq",__eq},
	{0, 0}
};

void on_lua_newState()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_newlib(L, methods);
	lua_setfield(L, LUA_REGISTRYINDEX, as3);
	
	AS3_Return(L);
}

void on_lua_close()
{
	lua_State* L;
	AS3_GetScalarFromVar(L, L);
	lua_close(L);
}

void on_lua_doString()
{
	lua_State* L;
	AS3_GetScalarFromVar(L, L);

	char* str = NULL;
	AS3_MallocString(str, str);
	int result = luaL_dostring(L, str);
	free(str);
	
	if(result != LUA_OK){
		var err = lua_tovar(L, -1);
		lua_pop(L, 1);
		_throw(new_Error(err));
	}
}

void on_lua_invoke()
{
	lua_State* L;
	AS3_GetScalarFromVar(L, L);

	char* name = NULL;
	AS3_MallocString(name, name);
	lua_getglobal(L, name);
	free(name);

	var argList;
	AS3_GetVarxxFromVar(argList, argList);

	int argCount = lua_pusharray(L, argList);
	
	if(lua_pcall(L, argCount, 1, 0)){
		var err = lua_tovar(L, -1);
		lua_pop(L, 1);
		_throw(new_Error(err));
		return;
	}

	var result = lua_tovar(L, -1);
	lua_pop(L, 1);

	AS3_DeclareVar(result, Object);
	AS3_CopyVarxxToVar(result, result);
	AS3_ReturnAS3Var(result);
}

void on_lua_setVar()
{
	lua_State* L;
	AS3_GetScalarFromVar(L, L);

	var value;
	AS3_GetVarxxFromVar(value, value);

	char* name = NULL;
	AS3_MallocString(name, name);

	lua_pushvar(L, value);
	lua_setglobal(L, name);

	free(name);
}

void on_lua_getVar()
{
	lua_State* L;
	AS3_GetScalarFromVar(L, L);

	char* name = NULL;
	AS3_MallocString(name, name);

	lua_getglobal(L, name);
	var result = lua_tovar(L, -1);
	lua_pop(L, 1);

	free(name);

	AS3_DeclareVar(result, Object);
	AS3_CopyVarxxToVar(result, result);
	AS3_ReturnAS3Var(result);
}