#include "Server.h"
Server::Server():
	server_socket(nullptr),
	client_socket(nullptr),
	ipproto(0),
	active(false),
	connected(false)
{}
Server::~Server() {
	Close();
}
bool Server::Start(int ipproto, std::string ip_address) {
	if (!active) {
		if (!(ipproto == IPPROTO_TCP || ipproto == IPPROTO_UDP)) {
			std::cerr << "Unknown IPPROTO type" << std::endl;
			return false;
		}
		this->ipproto = ipproto;
		// Init WinSock
		int result = WSAStartup(MAKEWORD(2, 2), &data);
		if (result != 0) {
			std::cerr << "WSAStartup failed: " << result << std::endl;
			return false;
		}
		// Getting address info
		saddr.sin_family = AF_INET;
		inet_pton(saddr.sin_family, ip_address.c_str(), &saddr.sin_addr);
		saddr.sin_port = htons((unsigned short)8200);
		saddri = nullptr;
		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		if (ipproto == IPPROTO_TCP) {
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
		}
		else if (ipproto == IPPROTO_UDP) {
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;
		}
		hints.ai_flags = AI_PASSIVE;
		std::string kek = std::to_string(ntohs(saddr.sin_port)).c_str();
		result = getaddrinfo(ip_address.c_str(), std::to_string(ntohs(saddr.sin_port)).c_str(), &hints, &saddri);
		if (result != 0) {
			std::cerr << "Getaddrinfo failed: " << result << std::endl;
			return false;
		}
		// Creating socket
		server_socket = new SOCKET(socket(saddri->ai_family, saddri->ai_socktype, saddri->ai_protocol));
		if (*server_socket == INVALID_SOCKET) {
			std::cerr << "Error at socket: " << WSAGetLastError() << std::endl;
			active = true; Close();
			return false;
		}
		// Set socket options
		int opt = true;
		result = setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
		if (result == SOCKET_ERROR) {
			std::cerr << "Set options failed with error: " << WSAGetLastError() << std::endl;
			active = true; Close();
			return false;
		}
		// Binding socket
		result = bind(*server_socket, saddri->ai_addr, (int)saddri->ai_addrlen);
		if (result == SOCKET_ERROR) {
			std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
			active = true; Close();
			return false;
		}
		// Unblock socket
		DWORD unblocking = 1;
		if (ioctlsocket(*server_socket, FIONBIO, &unblocking) != 0)
		{
			std::cerr << "Undlock failed with error: " << WSAGetLastError() << std::endl;
			active = true; Close();
			return false;
		}
		if (ipproto == IPPROTO_TCP) {
			// Init listen-socket
			if (listen(*server_socket, SOMAXCONN) == SOCKET_ERROR) {
				std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
				active = true; Close();
				return false;
			}
		}
		std::cout << "Server started on " << inet_ntoa(saddr.sin_addr) << ":" << std::to_string(ntohs(saddr.sin_port)) << std::endl;
		active = true;
		return true;
	}
	else {
		std::cout << "Server already started" << std::endl;
		return true;
	}
}
void Server::Close() {
	if (saddri != nullptr) freeaddrinfo(saddri); saddri = nullptr;
	if (client_socket != nullptr) {
		closesocket(*client_socket);
		delete client_socket;
		client_socket = nullptr;
	}
	if (server_socket != nullptr) {
		closesocket(*server_socket);
		delete server_socket;
		server_socket = nullptr;
	}
	active = false; connected = false;
	WSACleanup();
}
bool Server::Find(double timeout) {
	if (active) {
		// Clear last client's socket
		if (client_socket != nullptr) { closesocket(*client_socket); delete client_socket; client_socket = nullptr; }
		// Common variables
		char *buffer = new char[65536]; ZeroMemory(buffer, 65536);
		fd_set s_set = { 1,{ *server_socket } };
		timeval timeoutv = { (long)timeout / 1L, (long)(1000000 * std::fmod(timeout,1.0)) };
		// Waiting for connection TCP
		if (ipproto == IPPROTO_TCP) {
			FD_ZERO(&s_set);
			FD_SET(*server_socket, &s_set);
			std::cout << "Waiting for connection" << std::endl;
			int result = select(FD_SETSIZE, &s_set, (fd_set*)nullptr, (fd_set*)nullptr, &timeoutv);
			if (result == SOCKET_ERROR) {
				if (errno != EWOULDBLOCK) {
					std::cerr << "Socket error: " << WSAGetLastError() << std::endl;
					connected = false;
					delete[] buffer;
					return false;
				}
				std::cout << "No connections" << std::endl;
				connected = false;
				delete[] buffer;
				return false;
			}
			if (result > 0) {
				// Accepting connecting
				client_socket = new SOCKET(accept(*server_socket, nullptr, nullptr));
				std::cout << "Client's request found" << std::endl;
				if (*client_socket == INVALID_SOCKET) {
					std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
					delete client_socket; client_socket = nullptr;
					connected = false;
					delete[] buffer;
					return false;
				}
				// Unblock socket to client
				DWORD unblocking = 1;
				if (ioctlsocket(*client_socket, FIONBIO, &unblocking) != 0)
				{
					std::cerr << "Unblock failed with error: " << WSAGetLastError() << std::endl;
					active = true; Close();
					return false;
				}
				// Cleaning excess information
				do { result = recv(*client_socket, buffer, sizeof(buffer), 0); } while (result > 0);
				connected = true;
				delete[] buffer;
				return true;
			}
		}
		// Waiting for connection UDP
		else if (ipproto == IPPROTO_UDP) {
			sockaddr_in from;
			int from_length = sizeof(from);
			int result = recvfrom(*server_socket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_length);
			std::cout << "Received " << ((result != SOCKET_ERROR) ? std::to_string(result) : "no") << " bytes from "
				<< ((result != SOCKET_ERROR) ? (std::string(inet_ntoa(from.sin_addr)) + ":" + std::to_string(ntohs(from.sin_port))) : "nobody")
				<< std::endl;
			if (result != SOCKET_ERROR) {
				// Accepting connecting
				caddr = from;
				result = sendto(*server_socket, "Hi", sizeof("Hi"), 0, (sockaddr*)&caddr, sizeof(sockaddr_in));
				std::cout << "Sended " << result << " bytes to " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << std::endl;
				if (result != sizeof("Hi")) {
					std::cout << "Failed to send packet: return value = " << result << std::endl;
					connected = false;
					delete[] buffer;
					return false;
				}
				client_socket = new SOCKET(*server_socket);
				// Cleaning excess information
				do { result = recvfrom(*server_socket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_length); } while (result > 0);
				connected = true;
				delete[] buffer;
				return true;
			}
			Sleep(1000);
		}
		connected = false;
		delete[] buffer;
		return false;
	}
	else {
		std::cout << "Server not started" << std::endl;
		return false;
	}
}
bool Server::Connected() {
	return connected;
}
SOCKET* Server::GetClient() const {
	if (client_socket != nullptr) return client_socket;
	else return nullptr;
}
SOCKET* Server::GetSocket() const {
	if (server_socket != nullptr) return server_socket;
	else return nullptr;
}
void Server::Clear(SOCKET socket) {
	if (connected) {
		int result = 0; char *buffer = new char[65536]; ZeroMemory(buffer, 65536);
		do {
			if (ipproto == IPPROTO_TCP) result = recv(socket, buffer, sizeof(buffer), 0);
			else if (ipproto == IPPROTO_UDP) result = recvfrom(socket, buffer, sizeof(buffer), 0, 0, 0);
		} while (result > 0);
		delete[] buffer;
	}
}
int Server::Send(SOCKET socket, std::string info_type, std::string &info) {
	if (connected) {
		// Add size info
		if (info_type.find(' ') != std::string::npos) {
			std::cout << "Incorrect info type" << std::endl;
			return SOCKET_ERROR;
		}
		info = info_type + " " + std::to_string(info.size() + 1) + " " + info;
		// Send info
		int result = 0, sended = 0;
		if (ipproto == IPPROTO_TCP) {
			result = send(socket, info.c_str(), (int)info.size() + 1, 0);
			if (result == SOCKET_ERROR) {
				std::cout << "Connection lost" << std::endl; // Works with TCP only
				//connected = false;
				return result;
			}
			sended += result;
		}
		else if (ipproto == IPPROTO_UDP) {
			for (int i = 0; i < (int)ceil(((double)info.size() + 1.0) / (double)sizeof(size_t)); i++) {
				result = sendto(socket,
					info.substr(size_t(i * sizeof(size_t)), size_t((i + 1) * sizeof(size_t))).c_str(),
					sizeof(size_t), 0, (sockaddr*)&saddr, sizeof(sockaddr_in));
				sended += result;
			}
		}
		std::cout << "Sended " << sended << " bytes" << std::endl;
		return sended;
	}
	else return SOCKET_ERROR;
}
int Server::Recv(SOCKET socket, std::string &info_type, std::string &info) const {
	if (connected) {
		// Clean text
		info_type.clear();
		info.clear();
		// Buffer
		char *buffer = new char[65536]; ZeroMemory(buffer, 65536);
		// Address info
		sockaddr_in from;
		socklen_t from_length = sizeof(from);
		// Reveive result
		int result = 0;
		// Receive bytes total
		int received_text = 0,
			received_total = 0;
		// Receive stages
		bool info_type_end = false,
			info_size_end = false,
			info_end = false;
		// Symbols count
		int info_size = 0;
		// Buffer bytes enumeration
		int i = 0;
		// Receiving info
		while (!info_end) {
			// Receive size_t bytes of info
			// sizeof(size_t) == 4 (x32)
			// sizeof(size_t) == 8 (x64)
			if (ipproto == IPPROTO_TCP) result = recv(socket, buffer, sizeof(buffer), 0);
			else if (ipproto == IPPROTO_UDP) result = recvfrom(socket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_length);
			if (result == SOCKET_ERROR || result == 0) { info_type.clear(); info.clear(); delete[] buffer; return SOCKET_ERROR; }
			i = 0;
			// Receive info_type
			if (!info_type_end) {
				for (; i < sizeof(size_t); i++) {
					received_total++;
					if (buffer[i] != ' ') info_type += buffer[i];
					else {
						i++;
						info_type_end = true;
						break;
					}
					if (buffer[i] == '\0') { info_type.clear(); info.clear(); delete[] buffer; return SOCKET_ERROR; }
				}
			}
			// Getting size info
			if (!info_size_end) {
				for (; i < sizeof(size_t); i++) {
					received_total++;
					if (buffer[i] != ' ') info += buffer[i];
					else {
						info_size = atoi(info.c_str());
						info.clear();
						i++;
						info_size_end = true;
						break;
					}
					if (buffer[i] == '\0') { info_type.clear(); info.clear(); delete[] buffer; return SOCKET_ERROR; }
				}
			}
			// Receive info
			if (!info_end) {
				for (; i < sizeof(size_t); i++) {
					if (received_text <= info_size) {
						received_text++;
						info += buffer[i];
						if (received_text == info_size) {
							if (buffer[i] == '\0') { info_end = true; break; }
							else { info_type.clear(); info.clear(); delete[] buffer; return SOCKET_ERROR; }
						}
						if (buffer[i] == '\0') { info_type.clear(); info.clear(); delete[] buffer; return SOCKET_ERROR; }
					}
				}
			}
		}
		// Cleaning excess information
		do {
			if (ipproto == IPPROTO_TCP) result = recv(socket, buffer, sizeof(buffer), 0);
			else if (ipproto == IPPROTO_UDP) result = recvfrom(socket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_length);
		} while (result > 0);
		std::cout << "Received " << received_total << " bytes" << std::endl;
		delete[] buffer;
		return received_total;
	}
	else return SOCKET_ERROR;
}