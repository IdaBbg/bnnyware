#include "TC.hpp"

bool TC::ValidateMemoryRegion(void* ptr, size_t size, const char* regionName)
{
    if (!ptr) return false;

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0)
        return false;

    uintptr_t start = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t end = start + size;
    uintptr_t regionStart = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
    uintptr_t regionEnd = regionStart + mbi.RegionSize;

    return (start >= regionStart && end <= regionEnd) &&
        (mbi.State == MEM_COMMIT) &&
        ((mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) != 0);
}

bool TC::ValidateLuaState(lua_State* L)
{
    if (!ValidateMemoryRegion(L, sizeof(lua_State), "LuaState")) return false;
    if (!ValidateMemoryRegion(L->userdata, 0x100, "LuaState->userdata")) return false;
    return true;
}

bool TC::ValidateExecutionEnvironment(lua_State* L)
{
    if (!ValidateLuaState(L))
    {
        return false;
    }

    if (!ValidateMemoryRegion(L->top, sizeof(TValue), "LuaState->top"))
    {
        return false;
    }

    if (!ValidateMemoryRegion(L->base, sizeof(TValue), "LuaState->base"))
    {
        return false;
    }

    if (!ValidateMemoryRegion(L->ci, sizeof(CallInfo), "LuaState->ci"))
    {
        return false;
    }

    if (L->top < L->base)
    {
        return false;
    }

    return true;
}

bool TC::ValidateLuaClosure(lua_State* L, int index)
{
    if (!ValidateLuaState(L))
    {
        return false;
    }

    TValue* obj = reinterpret_cast<TValue*>(L->base + index);
    char indexBuffer[64];
    sprintf_s(indexBuffer, "Closure TValue at index %d", index);

    if (!ValidateMemoryRegion(obj, sizeof(TValue), indexBuffer))
    {
        return false;
    }

    if (obj->tt != LUA_TFUNCTION)
    {
        return false;
    }

    Closure* cl = reinterpret_cast<Closure*>(obj->value.gc);
    if (!ValidateMemoryRegion(cl, sizeof(Closure), "Closure object"))
    {
        return false;
    }

    return true;
}

lua_State* TC::DecryptLuaState(uintptr_t ScriptContext)
{
    if (!ValidateMemoryRegion(reinterpret_cast<void*>(ScriptContext), 0x1000, "ScriptContext"))
    {
        return nullptr;
    }

    *reinterpret_cast<BOOLEAN*>(ScriptContext + 0x870) = TRUE;

    uintptr_t StateLocation = ScriptContext + 0x210;
    const uint32_t* EncryptedComponents = reinterpret_cast<const uint32_t*>(StateLocation);

    uint32_t LowerBits = EncryptedComponents[0] ^ static_cast<uint32_t>(StateLocation);
    uint32_t UpperBits = EncryptedComponents[1] ^ static_cast<uint32_t>(StateLocation);

    uint64_t ReconstructedPointer = (static_cast<uint64_t>(UpperBits) << 32) | LowerBits;

    lua_State* DecryptedState = reinterpret_cast<lua_State*>(ReconstructedPointer);

    if (!ValidateLuaState(DecryptedState))
    {
        return nullptr;
    }

    return DecryptedState;
}

void TC::SetProtoCapabilities(Proto* Proto, uintptr_t* Capabilities)
{
    if (!TC::ValidateMemoryRegion(Proto, sizeof(Proto), "Proto object"))
    {
        return;
    }

    Proto->userdata = Capabilities;

    for (int i = 0; i < Proto->sizep; ++i)
    {
        if (!TC::ValidateMemoryRegion(Proto->p[i], sizeof(Proto), "Child Proto"))
        {
            continue;
        }
        SetProtoCapabilities(Proto->p[i], Capabilities);
    }
}

void TC::SetThreadCapabilities(lua_State* L, int Level, uintptr_t Capabilities)
{
    if (!ValidateLuaState(L))
    {
        return;
    }

    uintptr_t userdataAddr = reinterpret_cast<uintptr_t>(L->userdata);

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(userdataAddr + Offsets::ExtraSpace::Capabilities),
        sizeof(uintptr_t), "Capabilities offset"))
    {
        return;
    }

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(userdataAddr + Offsets::ExtraSpace::Identity),
        sizeof(int), "Identity offset"))
    {
        return;
    }

    *reinterpret_cast<uintptr_t*>(userdataAddr + Offsets::ExtraSpace::Capabilities) = Capabilities;
    *reinterpret_cast<int*>(userdataAddr + Offsets::ExtraSpace::Identity) = Level;
}

uintptr_t TC::GetDataModel()
{
    if (!ValidateMemoryRegion(reinterpret_cast<void*>(Offsets::DataModel::FakeDataModelPointer),
        sizeof(uintptr_t), "FakeDataModel pointer offset"))
    {
        return 0;
    }

    uintptr_t FakeDataModel = *reinterpret_cast<uintptr_t*>(Offsets::DataModel::FakeDataModelPointer);

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(FakeDataModel), 0x100, "FakeDataModel"))
    {
        return 0;
    }

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(FakeDataModel + Offsets::DataModel::FakeDataModelToDataModel),
        sizeof(uintptr_t), "DataModel offset from FakeDataModel"))
    {
        return 0;
    }

    uintptr_t DataModel = *reinterpret_cast<uintptr_t*>(FakeDataModel + Offsets::DataModel::FakeDataModelToDataModel);

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(DataModel), 0x1000, "DataModel"))
    {
        return 0;
    }

    return DataModel;
}

uintptr_t TC::GetScriptContext(uintptr_t DataModel)
{
    if (!ValidateMemoryRegion(reinterpret_cast<void*>(DataModel), 0x1000, "DataModel"))
    {
        return 0;
    }

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(DataModel + Offsets::DataModel::Children),
        sizeof(uintptr_t), "Children offset"))
    {
        return 0;
    }

    uintptr_t Children = *reinterpret_cast<uintptr_t*>(DataModel + Offsets::DataModel::Children);

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(Children), sizeof(uintptr_t), "Children array"))
    {
        return 0;
    }

    uintptr_t FirstChild = *reinterpret_cast<uintptr_t*>(Children);

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(FirstChild), 0x100, "First child"))
    {
        return 0;
    }

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(FirstChild + Offsets::DataModel::ScriptContext),
        sizeof(uintptr_t), "ScriptContext offset"))
    {
        return 0;
    }

    uintptr_t ScriptContext = *reinterpret_cast<uintptr_t*>(FirstChild + Offsets::DataModel::ScriptContext);

    if (!ValidateMemoryRegion(reinterpret_cast<void*>(ScriptContext), 0x1000, "ScriptContext"))
    {
        return 0;
    }

    return ScriptContext;
}