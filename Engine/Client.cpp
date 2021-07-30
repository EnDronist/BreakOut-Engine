#include "Client.h"
Client::Client():
	client_socket(nullptr),
	ipproto(0),
	active(false),
	connected(false)
{}
Client::~Client() {
	Close();
}
bool Client::Start(int ipproto) {
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
		// Creating socket
		client_socket = new SOCKET(socket(AF_INET, (ipproto == IPPROTO_TCP) ? SOCK_STREAM : SOCK_DGRAM, ipproto));
		if (*client_socket == INVALID_SOCKET) {
			std::cerr << "Error at socket: " << WSAGetLastError() << std::endl;
			active = true; Close();
			return false;
		}
		// Unblock socket mode
		DWORD unblocking = 1;
		if (ioctlsocket(*client_socket, FIONBIO, &unblocking) != 0)
		{
			std::cerr << "Unblock failed with error: " << WSAGetLastError() << std::endl;
			active = true; Close();
			return false;
		}
		// Set socket options (if FIONBIO == 0)
		/*int opt = 1000;
		result = setsockopt(*client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&opt, sizeof(opt));
		if (result == SOCKET_ERROR) {
		std::cerr << "Set options failed with error: " << WSAGetLastError() << std::endl;
		active = true; Close();
		return false;
		}*/
		std::cout << "Client started" << std::endl;
		active = true;
		return true;
	}
	else {
		std::cout << "Client already started" << std::endl;
		return true;
	}
}
void Client::Close() {
	if (client_socket != nullptr) { 
		closesocket(*client_socket);
		delete client_socket; 
		client_socket = nullptr;
	}
	active = false; connected = false;
	WSACleanup();
}
bool Client::Connect(double timeout, std::string ip_address) {
	if (active) {
		// Filling a hint structure
		saddr.sin_family = AF_INET;
		inet_pton(saddr.sin_family, ip_address.c_str(), &saddr.sin_addr);
		saddr.sin_port = htons((unsigned short)8200);
		std::cout << "Trying to connect to " << inet_ntoa(saddr.sin_addr) << ":" << ntohs(saddr.sin_port) << std::endl;
		// Connection to server TCP
		if (ipproto == IPPROTO_TCP) {
			// Set socket options (if FIONBIO == 0)
			DWORD recvtime = (DWORD)(1000000 * timeout);
			int result = setsockopt(*client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recvtime, sizeof(recvtime));
			if (result == SOCKET_ERROR) {
				std::cerr << "Set options failed with error: " << WSAGetLastError() << std::endl;
				active = true; Close();
				return false;
			}
			// Connect
			fd_set s_set = { 1,{ *client_socket } };
			timeval timeoutv = { (long)timeout / 1L, (long)(1000000 * std::fmod(timeout,1.0)) };
			FD_ZERO(&s_set);
			FD_SET(*client_socket, &s_set);
			result = connect(*client_socket, (sockaddr*)&saddr, sizeof(sockaddr_in));
			if (result == SOCKET_ERROR) {
				int result = select(FD_SETSIZE, &s_set, (fd_set*)nullptr, (fd_set*)nullptr, &timeoutv);
				if (result == 0) {
					std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
					connected = false;
					return false;
				}
				else {
					std::cout << "Connected to " << inet_ntoa(saddr.sin_addr) << ":" << ntohs(saddr.sin_port) << std::endl;
					connected = true;
					return true;
				}
			}
			/*while (true) {*/
				/*int result; char check[16]; int check_len = sizeof(check); int tries = 5;
				do {
					result = connect(*client_socket, (sockaddr*)&saddr, sizeof(sockaddr_in));
					if (result < 0) { Sleep(1); tries--; }
				} while ((errno == EALREADY && tries > 0) || (errno == EINPROGRESS && tries > 0));
				if (errno != EISCONN) {
					std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
					connected = false;
					return false;
				}
				*/
				/*if (result == EINPROGRESS || result == 0) {
					getsockopt(*client_socket, SOL_SOCKET, SO_ERROR, check, &check_len);
					if (check == "1") {
						int a = 0;
					}
					break;
				}
				else {
					std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
					connected = false;
					return false;
				}*/
				/*
				if (result == SOCKET_ERROR) {
					if (WSAGetLastError() == WSAEWOULDBLOCK) { continue; }
					else if (WSAGetLastError() == WSAEALREADY) { return false; }
					else if (WSAGetLastError() == WSAEISCONN) { break; }
					else {
						std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
						connected = false;
						return false;
					}
				}
				break;
				*/
			/*}*/
		}
		// Connection to server UDP
		if (ipproto == IPPROTO_UDP) {
			// Common variables
			sockaddr_in from;
			int from_length = sizeof(from);
			char *buffer = new char[65536]; ZeroMemory(buffer, 65536);
			int result = sendto(*client_socket, "Hi", sizeof("Hi"), 0, (sockaddr*)&saddr, sizeof(sockaddr_in));
			std::cout << "Sended " << result << " bytes to " << inet_ntoa(saddr.sin_addr) << ":" << ntohs(saddr.sin_port) << std::endl;
			if (result != sizeof("Hi")) {
				std::cout << "Failed to send packet: " << WSAGetLastError() << std::endl;
				connected = false;
				delete[] buffer;
				return false;
			}
			result = recvfrom(*client_socket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_length);
			std::cout << "Received " << ((result != SOCKET_ERROR) ? std::to_string(result) : "no") << " bytes from "
				<< ((result != SOCKET_ERROR) ? (std::string(inet_ntoa(from.sin_addr)) + ":" + std::to_string(ntohs(from.sin_port))) : "nobody")
				<< std::endl;
			if (result == SOCKET_ERROR) {
				Sleep(1000);
				delete[] buffer;
				return false;
			}
			saddr = from;
			connected = true;
			// Cleaning excess information
			do { result = recvfrom(*client_socket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_length); } while (result > 0);
			delete[] buffer;
			return true;
		}
		connected = false;
		return false;
	}
	else {
		std::cout << "Client not started" << std::endl;
		return false;
	}
}
bool Client::Connected() {
	return connected;
}
SOCKET* Client::GetSocket() const {
	if (client_socket != nullptr) return client_socket;
	else return nullptr;
}
void Client::Clear(SOCKET socket) {
	if (connected) {
		int result = 0; char *buffer = new char[65536]; ZeroMemory(buffer, 65536);
		do {
			if (ipproto == IPPROTO_TCP) result = recv(socket, buffer, sizeof(buffer), 0);
			else if (ipproto == IPPROTO_UDP) result = recvfrom(socket, buffer, sizeof(buffer), 0, 0, 0);
		} while (result > 0);
		delete[] buffer;
	}
}
int Client::Send(SOCKET socket, std::string info_type, std::string &info) {
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
int Client::Recv(SOCKET socket, std::string &info_type, std::string &info) const {
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