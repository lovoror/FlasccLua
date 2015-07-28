package
{
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.utils.getDefinitionByName;
	
	import snjdck.lua.lua_close;
	import snjdck.lua.lua_doString;
	import snjdck.lua.lua_getVar;
	import snjdck.lua.lua_invoke;
	import snjdck.lua.lua_newState;
	import snjdck.lua.lua_setVar;
	
	public class TestFlascc extends Sprite
	{
		[Embed(source="test.lua", mimeType="application/octet-stream")]
		static public const TEXT:Class;
		
		public var tf:TextField;
		
		public function TestFlascc()
		{
			trace("numChildren",numChildren);
			
			var L:int = lua_newState();
			lua_setVar(L, "new", createVar);
			lua_setVar(L, "getCls", getDefinitionByName);
			lua_setVar(L, "call", OnTest);
			lua_setVar(L, "json", parseInt);
			lua_setVar(L, "trace", trace);
			lua_setVar(L, "sprite", this);
			lua_setVar(L, "spriteCls", Sprite);
			lua_setVar(L, "aCls", TextFieldAutoSize);
			lua_setVar(L, "addChild", addChild);
			
			try{
				lua_doString(L, new TEXT().toString());
			}catch(e:Error){
				trace(e.getStackTrace());
			}
			tf.x = 100;
			trace("numChildren",numChildren);
			
			lua_doString(L, "sprite.x = 10");
			lua_doString(L, "trace('test', sprite.name == sprite.name)");
			lua_doString(L, "call('test', {1,2,3,4,5,6,key='the key'})");
			lua_doString(L, "call('test', 1,2,3,4,5,6)");
			lua_doString(L, "call('test', 'fuck',2,true,4,nil)");
			lua_doString(L, "trace('trace', 'fuck',2,true,4,nil)");
			lua_doString(L, "trace(json('0xff',16))");
			lua_doString(L, "a = 203;b = 'hello'");
			lua_doString(L, "function add2(x,y) return x+y end");
			lua_doString(L, "trace(add2(7,9))");

			trace(lua_getVar(L, 'a'));
			trace(lua_getVar(L, 'b'));
			trace(lua_getVar(L, '__as3__'));
			trace(lua_getVar(L, 'add2'));
			trace(lua_invoke(L, 'add2', [5,9]));
			
			lua_close(L);
		}
		
		private function createVar(clsName:Object, ...args):Object
		{
			var ref:Object = clsName is String ? getDefinitionByName(clsName as String) : clsName;
			if(ref is Function){
				return ref;
			}
			if(null == args){
				return new ref();
			}
			switch(args.length){
				case 0:
					return new ref();
				case 1:
					return new ref(args[0]);
				case 2:
					return new ref(args[0], args[1]);
			}
			return null;
		}
		
		private function OnTest(...args):void
		{
			trace("success", JSON.stringify(args));
			for each(var arg:Object in args){
				if(arg is Array){
					for(var key:String in arg){
						trace(key, arg[key]);
					}
				}
			}
		}
	}
}