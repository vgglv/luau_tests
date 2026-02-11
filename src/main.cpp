#include "Luau/Compiler.h"
#include "lualib.h"
#include <string>
#include <iostream>

int main() {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	std::string script_code = R"(
		print('Hello, world from lua')
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

	if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
		std::cerr << "luau_pcall runtime error: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return 1;
	}
	return 0;
}
