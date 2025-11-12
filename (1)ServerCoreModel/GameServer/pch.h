#pragma once



#define WIN32_LEAN_AND_MEAN         

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#endif

#pragma comment(lib, "mysqlcppconn.lib")

#include"CorePch.h"

#include "Vector2.h"
#include "DBFuncs.h"
#include "Protocol.h"

using ClientSessionRef = shared_ptr<class ClientSession>;
using PlayerRef = shared_ptr<class Player>;  
using RoomRef = shared_ptr<class Room>;
using CapsuleRoomRef = shared_ptr<class CapsuleRoom>;
using MultiRoomRef = shared_ptr<class MultiRoom>;
using NetObjectRef = shared_ptr<class NetObject>;
using HeightObjectRef = shared_ptr<class HeightObject>;

using PivotRef = shared_ptr<class Pivot>;
using EnergyRef = shared_ptr<class Energy>;
using LoosePivotRef = shared_ptr<class LoosePivot>;

/*----------------------
	  Compile Flag
----------------------*/
#define _MEMORY_COUNT


