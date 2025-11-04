#include "SC.hpp"


int ScriptsHandler(lua_State* L)
{
    if (!SharedVariables::ExecutionRequests.empty())
    {
        EXT::ExecuteScript(SharedVariables::BnnyThread, SharedVariables::ExecutionRequests.front());
        SharedVariables::ExecutionRequests.erase(SharedVariables::ExecutionRequests.begin());
    }
    Yield::RunYield();
    return 0;
}

void SetupExecution(lua_State* L)
{
    if (!TC::ValidateLuaState(L)) {
        return;
    }

    try {
        lua_getglobal(L, "game");
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            return;
        }
        lua_getfield(L, -1, "GetService");
        if (lua_isnil(L, -1)) {
            lua_pop(L, 2);
            return;
        }
        lua_pushvalue(L, -2);
        lua_pushstring(L, "RunService");
        if (lua_pcall(L, 2, 1, 0) != 0) {
            lua_pop(L, 2);
            return;
        }
        lua_getfield(L, -1, "Stepped");
        if (lua_isnil(L, -1)) {
            lua_pop(L, 3);
            return;
        }
        lua_getfield(L, -1, "Connect");
        if (lua_isnil(L, -1)) {
            lua_pop(L, 4);
            return;
        }
        lua_pushvalue(L, -2);
        lua_pushcclosure(L, ScriptsHandler, nullptr, 0);

        if (lua_pcall(L, 2, 0, 0) != 0) {
            lua_pop(L, 2);
            return;
        }

        lua_pop(L, 2);
    }
    catch (...) {
        lua_settop(L, 0);
    }
}

bool SC::SetupExploit()
{
    uintptr_t ScriptContext = TC::GetScriptContext(SharedVariables::RosaParksDataModel);

    if (!TC::ValidateMemoryRegion(reinterpret_cast<void*>(ScriptContext), 0x1000, "ScriptContext")) {
        return false;
    }

    lua_State* RobloxState = TC::DecryptLuaState(ScriptContext);

    if (!TC::ValidateLuaState(RobloxState)) {
        return false;
    }

    SharedVariables::BnnyThread = lua_newthread(RobloxState);

    if (!TC::ValidateLuaState(SharedVariables::BnnyThread)) {
        return false;
    }

    TC::SetThreadCapabilities(SharedVariables::BnnyThread, 8, Max_Caps);

    if (!TC::ValidateExecutionEnvironment(SharedVariables::BnnyThread)) {
        return false;
    }
    Env::StartEnv(SharedVariables::BnnyThread);
    SetupExecution(SharedVariables::BnnyThread);

    return true;

}

void SC::RequestExecution(std::string Script)
{
    SharedVariables::ExecutionRequests.push_back(Script);
}