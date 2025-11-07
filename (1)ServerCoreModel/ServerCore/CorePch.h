/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/


// Network Library
#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// Default
#include <Windows.h>
#include <iostream>
using namespace std;

// Main
#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

// Memroy
#include "Container.h"
#include "ObjectPool.h"
#include "TypeCast.h"
#include "Memory.h"
#include "RefCounting.h"

// Thread
#include "Lock.h"

// Network
#include "SendBuffer.h"
#include "Session.h"

// Job
#include "JobQueue.h"

// Utils
#include "WStringCoverter.h"

// DB
#include "MySql.h"