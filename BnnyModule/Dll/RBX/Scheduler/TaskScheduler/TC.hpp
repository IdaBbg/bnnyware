#pragma once
#include <Dll/Helper/Includes.hpp>


namespace TC
{
	bool ValidateMemoryRegion(void* ptr, size_t size, const char* regionName);
	bool ValidateLuaState(lua_State* L);
	bool ValidateExecutionEnvironment(lua_State* L);
	bool ValidateLuaClosure(lua_State* L, int index);
	void InitializeDebugConsole();
	lua_State* DecryptLuaState(uintptr_t ScriptContext);
	void SetProtoCapabilities(Proto* Proto, uintptr_t* Capabilities);
	void SetThreadCapabilities(lua_State* L, int Level, uintptr_t Capabilities);
	uintptr_t GetDataModel();
	uintptr_t GetScriptContext(uintptr_t DataModel);
	lua_State* DecryptLuaState(uintptr_t ScriptContext);
	void DebugPointer(const char* category, const char* component, const char* issue, const char* fix = nullptr);
	void DebugSuccess(const char* component, const char* message);
	void DebugInfo(const char* component, const char* message);


}