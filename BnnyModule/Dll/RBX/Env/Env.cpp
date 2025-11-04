#include "Env.hpp"
#include <Dll/Helper/Includes.hpp>
#include <Dll/RBX/Env/Libs/Misc.hpp>
#include <Dll/RBX/Env/Libs/Https.hpp>
#include <Dll/RBX/Env/Libs/Closures.hpp>





void Env::StartEnv(lua_State* L)
{
    Hooks::Initialize(L);
    Misc::RegisterLib(L);
    Http::RegisterLib(L);
    Closures::RegisterLib(L);

    luaL_sandboxthread(L);
    lua_newtable(L);
    lua_setglobal(L, "_G");
    lua_newtable(L);
    lua_setglobal(L, "shared");
}

