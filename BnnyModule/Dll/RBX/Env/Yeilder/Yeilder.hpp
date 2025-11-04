#pragma once
#include <Dll/Helper/Includes.hpp>

using Yielded = std::function<int(lua_State*)>;
namespace Yield
{
	int YieldExecution(lua_State* state, const std::function<Yielded()>& asyncCallback);
	void RunYield();
}