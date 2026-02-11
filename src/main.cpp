#include "Luau/Compiler.h"
#include "lua.h"
#include "lualib.h"
#include <string>
#include <iostream>

int main() {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	std::string script_code = R"(
		print('Hello, world from lua')

		function greet(name)
			print("Greetings, ", name)
		end
	)";

	Luau::CompileOptions compile_options;
	compile_options.optimizationLevel = 2;
	compile_options.debugLevel = 1;

	std::string bytecode = Luau::compile(script_code, compile_options);
	if (luau_load(L, "asd", bytecode.data(), bytecode.size(), 0) != LUA_OK) {
		std::cerr << "luau_load error: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return 1;
	}

	lua_newtable(L); // this will be env
	lua_newtable(L); // metatable

	lua_getglobal(L, "_G"); // getting global table 
	lua_setfield(L, -2, "__index"); // copying _G to __index in metatable
	lua_setmetatable(L, -2); // apply metatable to environment
	
	lua_setfenv(L, -2); // set environment for loaded function (luau_load before)
	lua_getfenv(L, -1); // push environment back to stack
	int env_ref = lua_ref(L, -1); // create reference to environment
	lua_pop(L, 1); // we are popping here because lua_ref made our stack point to nil
	
	std::cout << "Env ref is: " << env_ref << "\n";
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		std::cerr << "luau_pcall runtime error: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return 1;
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, env_ref); // get environment from registry
	//std::cout << "Type of greet: " << lua_typename(L, lua_type(L, -1)) << "\n";
	lua_getfield(L, -1, "greet"); // get greet function
	int greet_ref = LUA_NOREF;
	if (lua_isfunction(L, -1)) {
		greet_ref = lua_ref(L, -1); // create reference to function
	} else {
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_rawgeti(L, LUA_REGISTRYINDEX, greet_ref); // get greet func from registry
	lua_pushstring(L, "Sarah"); // push argument to function

	if (lua_pcall(L, 1, 0, 0) != LUA_OK) // call with 1 arg, 0 results
	{
		std::cerr << "luau_pcall(greet) runtime error: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
	}
	return 0;
}
