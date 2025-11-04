#pragma once

#include <Dll/Helper/Includes.hpp>
#include <Dll/RBX/Scheduler/TaskScheduler/TC.hpp>

namespace EXT
{
	std::string CompileScript(std::string Source);
	void ExecuteScript(lua_State* l, const std::string& script, bool use_stack_guard = false);
}