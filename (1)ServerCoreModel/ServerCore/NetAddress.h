/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once

/*-------------------
	  NetAddress
-------------------*/
class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(WString ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	WString GetIpAddress();
	uint16 GetPort() const { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};