#pragma once
#include <string>
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
class Server {
public:
	Server();
	virtual ~Server();
	bool Start(int IPPROTO, std::string ip_address = "127.0.0.1");
	void Close();
	bool Find(double timeout);
	bool Connected();
	SOCKET* GetClient() const;
	SOCKET* GetSocket() const;
	void Clear(SOCKET socket);
	int Send(SOCKET socket, std::string info_type, std::string &info);
	int Recv(SOCKET socket, std::string &info_type, std::string &info) const;
private:
	WSADATA data;
	SOCKET *server_socket;
	SOCKET *client_socket;
	addrinfo *saddri;
	sockaddr_in saddr, caddr;
	int ipproto;
	bool active;
	bool connected;
};
