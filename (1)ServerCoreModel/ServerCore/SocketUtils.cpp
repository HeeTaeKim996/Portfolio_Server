#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250525
--------------------------------------*/

#include "SocketUtils.h"

/*----------------------
	   SocketUtils
----------------------*/

LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT &wsaData) == 0);


	// 런타임에 주소 얻어오는 API
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX,
		reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));

	Close(dummySocket);
}

void SocketUtils::Clear()
{
	::WSACleanup();
}

bool SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	return ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT &bytes, NULL,
		NULL) != SOCKET_ERROR;
}

SOCKET SocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	/* ○ ::WSASocket
		- 4, 5	: 일반적으로 NULL, 0
		- 6		: 비동기 소켓 I/O 사용하겠다는 플래그 ( IOCP (비동기 논블로킹) + 비동기 블로킹(스레드 점유) 둘 다 사용) 
		  -> WSARecv, WSASend, AcceptEx, ConnectEx 등 함수 사용 가능
	*/
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onOff, int16 linger)
{
	LINGER option;
	option.l_onoff = onOff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
	/* ○ LINGER
		- l_onoff	:	1일시, 링거 사용. 0일시, 링거 사용 안함
		- l_linger	:	링거 대기시간
		==>> CloseSocket 시라도, 링거대기시간 만큼 데이터 전송 시도
	*/
}

bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
	/* ○ SO_REUSEADDR
		- 서버 크래시 때, 커널은 TCP연결 안전을 위해 TIME_WAIT 상태를 몇초간 유지.
		- 이 때 서버를 재시작 하여, Bind를 할 때 오류 발생 가능

		- SO_REUSEADDR 설정시, 크래시후 즉시 재시작해도 오류 발생 안함

	   => 개발 단계에서 주로 사용. 
	   ※ 주의. Bind 전에 위 설정을 해야, 적용 가능
	*/
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
	/* ○ TCP_NODELAY
		- 디펄트는 flag가 false로, TCP 송신 때 데이터가 충분히 쌓여야 송신
		- 게임 서버에서는 데이터가 작아도 즉시 보내야 하기 때문에, flag 를 true로 수동 처리

		@@@@ C# 클라이언트 네트워크 라이브러리 만들 때에도, socket.NoDelay = true; 처리 필요 @@@@
	*/
}

bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
	// listenSocket에 설정했던 SetSockOpt 설정들을, Accept로 생성된 클라이언트 소켓에도 그대로 적용
}

bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
{
	return ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN)) 
		!= SOCKET_ERROR;
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress)) != SOCKET_ERROR;
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return ::listen(socket, backlog) != SOCKET_ERROR;
}

void SocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
	{
		::closesocket(socket);
		socket = INVALID_SOCKET;
	}
}

// ※ SOCKET은 정수형 타입으로, 커널에서 소켓을 구분하기 위한 식별자로, 함수의 매개변수로 받을 때 복사로 받는게 연산에 좋음