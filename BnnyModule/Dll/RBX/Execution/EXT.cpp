#include "EXT.hpp"


class BytecodeEncoder : public Luau::BytecodeEncoder
{
	inline void encode(uint32_t* data, size_t count) override
	{
		for (auto i = 0; i < count;)
		{
			uint8_t Opcode = LUAU_INSN_OP(data[i]);
			const auto LookupTable = reinterpret_cast<BYTE*>(Offsets::OpcodeLookupTable);
			uint8_t FinalOpcode = Opcode * 227;
			FinalOpcode = LookupTable[FinalOpcode];

			data[i] = (FinalOpcode) | (data[i] & ~0xFF);
			i += Luau::getOpLength(static_cast<LuauOpcode>(Opcode));
		}
	}
};

std::string EXT::CompileScript(std::string Source)
{
	auto BytecodeEncoding = BytecodeEncoder();
	static const char* CommonGlobals[] = { "Game", "Workspace", "game", "plugin", "script", "shared", "workspace", "_G", "_ENV", nullptr };

	Luau::CompileOptions Options;
	Options.debugLevel = 1;
	Options.optimizationLevel = 1;
	Options.mutableGlobals = CommonGlobals;
	Options.vectorLib = "Vector3";
	Options.vectorCtor = "new";
	Options.vectorType = "Vector3";

	return Luau::compile(Source, Options, {}, &BytecodeEncoding);
}

void EXT::ExecuteScript(lua_State* l, const std::string& script, bool use_stack_guard) {
    if (script.empty())
        return;

    int original_top = lua_gettop(l);

    if (!use_stack_guard) {
        lua_State* thread = lua_newthread(l);
        lua_pop(l, 1);
        luaL_sandboxthread(thread);

        auto bytecode = EXT::CompileScript(script);
        if (luau_load(thread, "@lolaBnny", bytecode.c_str(), bytecode.size(), 0) != LUA_OK) {
            if (const char* err = lua_tostring(thread, -1))
                Roblox::Print(0, "%s", err);
            lua_pop(thread, 1);
            lua_settop(thread, 0);
            lua_settop(l, original_top);
            return;
        }

        if (auto closure = (Closure*)(lua_topointer(thread, -1)); closure && closure->l.p)
            TC::SetProtoCapabilities(closure->l.p, &Max_Caps);

        lua_getglobal(l, "task");
        lua_getfield(l, -1, "defer");
        lua_remove(l, -2);
        lua_xmove(thread, l, 1);

        if (lua_pcall(l, 1, 0, 0) != LUA_OK) {
            if (const char* err = lua_tostring(l, -1))
                Roblox::Print(0, "%s", err);
            lua_pop(l, 1);
        }

        lua_settop(thread, 0);
        lua_settop(l, original_top);
        return;
    }

    struct StackGuard {
        lua_State* L;
        int top;
        StackGuard(lua_State* s) : L(s), top(lua_gettop(s)) {}
        ~StackGuard() { if (L) lua_settop(L, top); }
    };

    StackGuard guard(l);

    lua_State* thread2 = lua_newthread(l);
    lua_pop(l, 1);
    luaL_sandboxthread(thread2);

    auto bytecode2 = EXT::CompileScript(script);
    if (luau_load(thread2, "@lolaBnny", bytecode2.c_str(), bytecode2.size(), 0) != LUA_OK) {
        if (const char* err = lua_tostring(thread2, -1))
            Roblox::Print(0, "%s", err);
        lua_pop(thread2, 1);
        return;
    }

    if (auto obj = lua_topointer(thread2, -1); obj) {
        if (auto closure = (Closure*)obj; closure && closure->l.p)
            TC::SetProtoCapabilities(closure->l.p, &Max_Caps);
    }

    lua_pushcfunction(thread2, reinterpret_cast<lua_CFunction>(Roblox::TaskDefer), "@lolaBnny");
    lua_insert(thread2, 1);

    lua_xmove(thread2, l, 1);

    if (lua_pcall(l, 1, 0, 0) != LUA_OK) {
        if (const char* err = lua_tostring(l, -1))
            Roblox::Print(0, "%s", err);
        lua_pop(l, 1);
    }
}
