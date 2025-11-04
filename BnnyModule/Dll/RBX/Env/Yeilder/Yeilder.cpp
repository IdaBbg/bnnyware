#include "Yeilder.hpp"


void Yield::RunYield()
{
    if (SharedVariables::YieldQueue.size() > 0)
    {
        auto pendingOperation = SharedVariables::YieldQueue.front();
        SharedVariables::YieldQueue.pop();
        pendingOperation();
    }
}

int Yield::YieldExecution(lua_State* state, const std::function<Yielded()>& asyncCallback)
{
    lua_pushthread(state);
    int threadReference = lua_ref(state, -1);
    lua_pop(state, 1);

    auto backgroundTask = std::thread([state, threadReference, asyncCallback]()
        {
            Yielded resumeCallback = asyncCallback();

            auto completionHandler = [state, threadReference, resumeCallback]()
                {
                    WeakThreadRef threadRefData = { state };
                    lua_pushthread(state);
                    threadRefData.ThreadRef = threadReference;
                    lua_pop(state, 1);

                    WeakThreadRef* threadRefPtr = &threadRefData;

                    uintptr_t contextAddress = reinterpret_cast<uintptr_t>(
                        state->userdata->SharedExtraSpace->ScriptContext);

                    DebuggerResult debugInfo = { 0 };
                    Roblox::ScriptContextResume(
                        contextAddress + Offsets::Luau::ScriptContextResume,
                        &debugInfo,
                        &threadRefPtr,
                        resumeCallback(state),
                        false,
                        nullptr
                    );

                    lua_unref(state, threadReference);
                };

            SharedVariables::YieldQueue.push(completionHandler);
        });

    backgroundTask.detach();

    return lua_yield(state, 0);
}
