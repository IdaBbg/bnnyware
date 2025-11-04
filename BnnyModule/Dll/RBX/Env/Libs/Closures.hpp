#include <Dll/Helper/Includes.hpp>

namespace Closures
{
    int loadstring(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const char* source = lua_tostring(L, 1);
        const char* chunkName = luaL_optstring(L, 2, "");

        std::string bytecode = EXT::CompileScript(source);
        if (bytecode.empty())
        {
            lua_pushnil(L);
            lua_pushstring(L, "Compilation failed");
            return 2;
        }

        if (luau_load(L, chunkName, bytecode.data(), bytecode.size(), 0) != LUA_OK)
        {
            lua_pushnil(L);
            lua_insert(L, -2);
            return 2;
        }

        TValue* obj = const_cast<TValue*>(luaA_toobject(L, -1));
        if (obj && obj->tt == LUA_TFUNCTION)
        {
            Closure* closure = clvalue(obj);
            if (closure && closure->isC == 0 && closure->l.p)
            {
                TC::SetProtoCapabilities(closure->l.p, &Max_Caps);
            }
        }

        lua_setsafeenv(L, LUA_GLOBALSINDEX, false);
        return 1;
    }

    void RegisterLib(lua_State* L)
    {
        Helper::RegisterGlobalFunction(L, Closures::loadstring, "loadstring");
    }
}