#pragma once



class Lobby : public JobQueue
{
public:
	static void Init();


public:
	Lobby();

	void AcceptClient(ClientSessionRef client);
	void RemoveClient(ClientSessionRef client);
	void MakeCapsuleRoom(ClientSessionRef client);
	void RemoveCapsuleRoom(uint64 roomId);

public:
	void RemoveMultiRoom(MultiRoomRef multiRoom) { _multiRooms.erase(multiRoom); }


public:
	void OnClientDisconnected(ClientSessionRef client);

public:
	uint64 GetLobbysPlayersCount() { return _clients.size(); }
	uint64 GetAllRoomsPlayersCount();
	uint32 CapsuleRoomCount() { return _capsuleRooms.size(); }


public:
	void InformLobbyInfos();

private:
	Set<ClientSessionRef> _clients;
	Map<uint64, CapsuleRoomRef> _capsuleRooms;
	Set<MultiRoomRef> _multiRooms;


/*---------------------
  	  RoomMatching
---------------------*/
public:
	void Match(CapsuleRoomRef matchingRoom, UINT32 score);

	inline void FinishMatching(const CapsuleRoomRef& finishingRoom)
	{
		weak_ptr<CapsuleRoom> weak = finishingRoom;
		_matchingRooms.erase(weak);
	}



private:
	Map<weak_ptr<CapsuleRoom>, UINT32, std::owner_less<weak_ptr<CapsuleRoom>>> _matchingRooms;
};
extern shared_ptr<Lobby> GLobby;



