#include <Dll/Helper/Includes.hpp>

namespace Misc
{
    int getobjects(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TUSERDATA);
        luaL_checktype(L, 2, LUA_TSTRING);

        lua_getglobal(L, "game");
        lua_getfield(L, -1, "GetService");
        lua_pushvalue(L, -2);
        lua_pushstring(L, "InsertService");
        lua_call(L, 2, 1);
        lua_remove(L, -2);

        lua_getfield(L, -1, "LoadLocalAsset");

        lua_pushvalue(L, -2);
        lua_pushvalue(L, 2);
        lua_pcall(L, 2, 1, 0);

        if (lua_type(L, -1) == LUA_TSTRING)
            luaL_error(L, lua_tostring(L, -1));

        lua_createtable(L, 1, 0);
        lua_pushvalue(L, -2);
        lua_rawseti(L, -2, 1);

        lua_remove(L, -3);
        lua_remove(L, -2);

        return 1;
    }

    int getgenv(lua_State* L)
    {
        lua_pushvalue(L, LUA_ENVIRONINDEX);
        return 1;
    }


    int identifyexecutor(lua_State* L)
    {
        lua_pushstring(L, "BnnyWare");
        lua_pushstring(L, "1.0");

        return 2;
    };

    int getexecutorname(lua_State* L)
    {
        lua_pushstring(L, "BnnyWare");

        return 1;
    };

    inline int getreg(lua_State* L)
    {
        lua_pushvalue(L, LUA_REGISTRYINDEX);

        return 1;
    };

    inline int getrawmetatable(lua_State* L)
    {
        if (L->userdata == nullptr)
            return 0;
        luaL_checkany(L, 1);
        if (!lua_getmetatable(L, 1))
            lua_pushnil(L);
        return 1;
    };

    inline int setrawmetatable(lua_State* L)
    {
        if (L->userdata == nullptr)
            return 0;
        luaL_checkany(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_setmetatable(L, 1);
        lua_pushvalue(L, 1);
        return 1;
    };


    void RegisterLib(lua_State* L)
    {
        Helper::RegisterGlobalFunction(L, Misc::getobjects, "getobjects");
        Helper::RegisterGlobalFunction(L, Misc::getgenv, "getgenv");
        Helper::RegisterGlobalFunction(L, Misc::getexecutorname, "getexecutorname");
        Helper::RegisterGlobalFunction(L, Misc::identifyexecutor, "identifyexecutor");
        Helper::RegisterGlobalFunction(L, Misc::getreg, "getreg");
        Helper::RegisterGlobalFunction(L, Misc::getrawmetatable, "getrawmetatable");
        Helper::RegisterGlobalFunction(L, Misc::setrawmetatable, "setrawmetatable");

    }




}



