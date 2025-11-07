#pragma once

class ClientSession;

class ClientSessionManager
{
public:
	static void Init();
	void Add(ClientSessionRef session);
	void Remove(ClientSessionRef session);
	void Broadcast(SendBufferRef sendBufferRef);

public:
	uint64 PlayersCount() { return _sessions.size(); }

private:
	USE_LOCK;
	Set<ClientSessionRef> _sessions;
};

extern ClientSessionManager* GSessionManager;

