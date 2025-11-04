#pragma once
#include "Includes.hpp"

namespace Helper
{
    inline void RegisterGlobalFunction(lua_State* L, lua_CFunction Function, const char* FunctionName)
    {
        lua_pushcclosure(L, Function, FunctionName, 0);
        lua_setfield(L, LUA_GLOBALSINDEX, FunctionName);
    }

    inline void RegisterTableFunction(lua_State* L, lua_CFunction Function, const char* FunctionName)
    {
        lua_pushcclosure(L, Function, FunctionName, 0);
        lua_setfield(L, -2, FunctionName);
    }

	inline bool IsInGame(uintptr_t DataModel)
	{
		auto CheckGameState = [](uintptr_t addr) -> bool {
			return *reinterpret_cast<uintptr_t*>(addr) == 31;
			};

		return CheckGameState(DataModel + Offsets::DataModel::GameLoaded);
	}
}