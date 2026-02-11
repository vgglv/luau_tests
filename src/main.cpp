#include "Luau/Compiler.h"
#include "lua.h"
#include "lualib.h"
#include <string>
#include <iostream>

int main() {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	// stack: [empty]

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
	// after luau_load:
	// STACK: [function]
	// so any call to -1 will call function, right?

	lua_newtable(L);
	// STACK: [function, table(env)]

	lua_newtable(L);
	// STACK: [function, table(env), table(metatable)]

	lua_getglobal(L, "_G"); // getting global table 
	// STACK: [function, table(env), table(metatable), table(_G_global)]

	lua_setfield(L, -2, "__index");
	// 1. Pops the _G_global table
	// 2. Sets _G_global to table(metatable)
	// STACK: [function, table(env), table(metatable_with___index)]

	lua_setmetatable(L, -2);
	// 1. Pops metatable
	// 2. Sets metatable to a table(env) (at index -2)
	// STACK: [function, table(env_with_metatable)]
	
	lua_setfenv(L, -2);
	// 1. Sets env_with_metatable as environment for the function
	// 2. Pops env_with_metatable
	// STACK: [function_with_env]

	lua_getfenv(L, -1);
	// 1. Gets environment of the function_with_env
	// 2. Pushes env_with_metatable back to stack
	// STACK: [function_with_env, env_with_metatable]

	int env_ref = lua_ref(L, -1);
	// 1. Creates a reference to the item at the top of stack (env_with_metatable)
	// 2. DOES NOT pop the item from the stack
	// STACK: [function_with_env, env_with_metatable]

	lua_pop(L, 1);
	// 1. Pops the top item on the stack, which is env_with_metatable
	// STACK: [function_with_env]
	
	std::cout << "Type of function_with_env: " << lua_typename(L, lua_type(L, -1)) <<"\n";
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		std::cerr << "luau_pcall runtime error: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return 1;
	}
	// 1. Calls function_with_env with 0 args, 0 returns
	// 2. Function is popped from stack during execution
	// STACK: [nil]
	
	lua_rawgeti(L, LUA_REGISTRYINDEX, env_ref);
	// 1. Gets env_with_metatable
	// 2. Pushes it to stack
	// STACK: [env_with_metatable]

	lua_getfield(L, -1, "greet");
	// 1. Gets 'greet' field from env_with_metatable
	// 2. Pushes it to stack
	// STACK: [env_with_metatable, greet_function]

	int greet_ref = LUA_NOREF;
	if (lua_isfunction(L, -1)) {
		greet_ref = lua_ref(L, -1);
		// 1. Creates a reference to the item at the top of stack (greet_function)
		// 2. DOES NOT POP the item from the stack
		// STACK: [env_with_metatable, greet_function]
		lua_pop(L, 1);
		// 1. Pops the first item from the stack
		// STACK: [env_with_metatable]
	} else {
		lua_pop(L, 1);
		// 1. Pops the first item from the stack
		// STACK: [env_with_metatable]
	}
	lua_pop(L, 1);
	// 1. Pops the first item from the stack
	// STACK: [nil]

	lua_rawgeti(L, LUA_REGISTRYINDEX, greet_ref);
	// 1. Gets greet_ref
	// 2. Pushes it to stack
	// STACK: [greet_function]

	lua_pushstring(L, "Sarah");
	// 1. Pushes string to a stack
	// STACK: [greet_function, "Sarah"]

	if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
		std::cerr << "luau_pcall(greet) runtime error: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
	}
	// 1. Calls greet_function with 1 arg, 0 returns
	// 2. greet_function is popped from stack during execution
	// STACK: [nil]
	std::cout << "Type of top stack is: " << lua_typename(L, lua_type(L, -1)) <<"\n";
	return 0;
}
