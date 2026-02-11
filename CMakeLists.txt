cmake_minimum_required(VERSION 3.10)
project(luau_tests)

add_subdirectory(luau-0.708)

add_executable(env-test "src/main.cpp")

set(LUAU_DIR "${CMAKE_SOURCE_DIR}/luau-0.708")

target_include_directories(env-test PUBLIC 
	"${CMAKE_SOURCE_DIR}/VM/include"
	"${CMAKE_SOURCE_DIR}/Compiler/include"
	"${CMAKE_SOURCE_DIR}/Ast/include"
	"${CMAKE_SOURCE_DIR}/Common/include"
	"${CMAKE_SOURCE_DIR}/Config/include"
)

target_link_libraries(env-test PUBLIC Luau.VM Luau.Compiler Luau.Ast Luau.Common Luau.Config)
