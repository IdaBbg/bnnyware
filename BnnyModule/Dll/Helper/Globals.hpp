#pragma once 
#include "Includes.hpp"

inline uintptr_t Max_Caps = 0xFFFFFFFFFFFFFFFF;
namespace SharedVariables
{
    inline uintptr_t RosaParksDataModel;
    inline lua_State* BnnyThread;
    inline std::vector<std::string> ExecutionRequests;
    inline std::queue<std::function<void()>> YieldQueue;
}