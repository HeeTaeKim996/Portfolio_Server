/* -------------------------------------
	  (1)ServerCoreModel_250526
--------------------------------------*/

#pragma once

#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;

/*-------------------
	   Listener
-------------------*/
class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();

public:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

private:
	SOCKET _socket = INVALID_SOCKET;
	Vector<AcceptEvent*> _acceptEvents;
	ServerServiceRef _service;
};