#pragma once



/*---------------------
	 ServerSession
---------------------*/
class ServerSession : public PacketSession
{
public:

	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override;

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len  = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};

