#pragma once

#include <Windows.h>

struct lua_State;
#define REBASE(Address) (Address + reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr)))

struct DebuggerResult
{
    std::int32_t Result;
    std::int32_t Unk[4];
};

struct WeakThreadRef
{
    std::atomic<std::int32_t> Refs;
    lua_State* L;
    std::int32_t ThreadRef;
    std::int32_t ObjectId;
    std::int32_t Unk1;
    std::int32_t Unk2;

    WeakThreadRef(lua_State* L) : Refs(0), L(L), ThreadRef(0), ObjectId(0), Unk1(0), Unk2(0) {}
};

namespace Offsets
{
    const uintptr_t Print = REBASE(0x190FDE0);
    const uintptr_t TaskDefer = REBASE(0x14D1930);
    const uintptr_t RawScheduler = REBASE(0x755F6E8);
    const uintptr_t OpcodeLookupTable = REBASE(0x57A6AC0);
    const uintptr_t ScriptContextResume = REBASE(0x128FC10);

    namespace Luau
    {
        const uintptr_t ScriptContextResume = 0x7A8;

        const uintptr_t LuaD_Throw = REBASE(0x379CCF0);
        const uintptr_t Luau_Execute = REBASE(0x37A4540);
        const uintptr_t LuaO_NilObject = REBASE(0x5303228);
        const uintptr_t LuaH_DummyNode = REBASE(0x5302C48);
    }

    namespace DataModel
    {
        const uintptr_t Children = 0x60;
        const uintptr_t GameLoaded = 0x5F0;
        const uintptr_t ScriptContext = 0x3D0;
        const uintptr_t FakeDataModelToDataModel = 0x1C0;

        const uintptr_t FakeDataModelPointer = REBASE(0x74287C8);
    }

	namespace ExtraSpace
	{
		const uintptr_t Identity = 0x30;
		const uintptr_t Capabilities = 0x48;
	}
}

namespace Roblox
{
    inline auto TaskDefer = (int(__fastcall*)(lua_State*))Offsets::TaskDefer;
    inline auto Print = (uintptr_t(__fastcall*)(int, const char*, ...))Offsets::Print;
    inline auto Luau_Execute = (void(__fastcall*)(lua_State*))Offsets::Luau::Luau_Execute;
    inline auto LuaD_Throw = (void(__fastcall*)(lua_State*, int))Offsets::Luau::LuaD_Throw;
    inline auto ScriptContextResume = (int(__fastcall*)(int64_t, DebuggerResult*, WeakThreadRef**, int32_t, bool, const char*))Offsets::ScriptContextResume;
}


