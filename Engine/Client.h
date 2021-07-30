#pragma once
#include <string>
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
class Client {
public:
	Client();
	virtual ~Client();
	bool Start(int IPPROTO);
	void Close();
	bool Connect(double timeout, std::string ip_address = "127.0.0.1");
	bool Connected();
	SOCKET* GetSocket() const;
	void Clear(SOCKET socket);
	int Send(SOCKET socket, std::string info_type, std::string &info);
	int Recv(SOCKET socket, std::string &info_type, std::string &info) const;
private:
	WSADATA data;
	SOCKET *client_socket;
	sockaddr_in saddr;
	int ipproto;
	bool active;
	bool connected;
};