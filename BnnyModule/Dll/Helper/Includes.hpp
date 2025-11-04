#pragma once


// Windows includes
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <shellapi.h>
#include <psapi.h>
#include <winnt.h>
#include <winternl.h>
#include <iphlpapi.h>
#include <wininet.h>
#include <DbgHelp.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "DbgHelp.lib")


#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <map>
#include <random>
#include <queue>
#include <unordered_map>
#include <regex>
#include <fstream>
#include <filesystem>
#include <format>
#include <mutex>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <set>

#include <cpr/cpr.h>
#include <Dependencies/nlohmann/json.hpp>


#include <lua.h>
#include <lstate.h>
#include <lualib.h>
#include <lstring.h>
#include <lfunc.h>
#include <lmem.h>
#include <lapi.h>
#include <lgc.h>
#include <ltable.h>

#include <Luau/BytecodeBuilder.h>
#include <Luau/BytecodeUtils.h>
#include <Luau/Compiler.h>


//Dll Includes

#include <Dll/RBX/Scheduler/Scheduler/SC.hpp>
#include <Dll/RBX/Scheduler/TaskScheduler/TC.hpp>
#include <Dll/RBX/Execution/EXT.hpp>
#include <Dll/Helper/Globals.hpp>
#include <Dll/Helper/Helper.hpp>
#include <Dll/Coms/Coms.hpp>
#include <Dll/RBX/Env/Yeilder/Yeilder.hpp>




