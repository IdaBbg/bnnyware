#include <Dll/Helper/Includes.hpp>
#include <Dll/RBX/Execution/EXT.hpp>
#include <Dll/RBX/Scheduler/Scheduler/SC.hpp>
#include <Dll/RBX/Scheduler/TaskScheduler/TC.hpp>
#include <Dll/Coms/Coms.hpp>




extern "C" __declspec(dllexport)
LRESULT NextHook(int code, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(nullptr, code, wParam, lParam);
}


void MainThread()
{
    Coms::Init();

    uintptr_t previousDataModel = 0;
    const auto checkDelay = std::chrono::milliseconds(1000);

    while (true)
    {
        uintptr_t activeDataModel = TC::GetDataModel();

        if (!TC::ValidateMemoryRegion(reinterpret_cast<void*>(activeDataModel), 0x1000, "ActiveDataModel"))
        {
            std::this_thread::sleep_for(checkDelay);
            continue;
        }

        bool hasDataModelChanged = (previousDataModel != activeDataModel);

        if (hasDataModelChanged)
        {
            if (!Helper::IsInGame(activeDataModel))
            {
                std::this_thread::sleep_for(checkDelay);
                continue;
            }

            previousDataModel = activeDataModel;
            SharedVariables::RosaParksDataModel = activeDataModel;

            if (!SharedVariables::ExecutionRequests.empty())
            {
                SharedVariables::ExecutionRequests.clear();
            }

            if (!SC::SetupExploit())
            {
                std::this_thread::sleep_for(checkDelay);
                continue;
            }

            SC::RequestExecution("print(\"Bnny Loaded Te he\")");
        }

        std::this_thread::sleep_for(checkDelay);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {

        DisableThreadLibraryCalls(hModule);
        std::thread(MainThread).detach();
    }
    return TRUE;
}


//Made By lola Bnny
