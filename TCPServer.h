#pragma once

#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class TCPServer;

typedef void(*MessageReceivedHandler)(TCPServer* listener, int socketID, std::string msg);

class TCPServer 
{
public:
	TCPServer();
	TCPServer(std::string ipAddress, int port);
	~TCPServer();

	void sendMsg(int clientSocket, std::string msg);
	bool initWinsock();
	void run();
	void cleanupWinsock();


private:
	SOCKET createSocket();
	std::string listenerIPAddress;
	int listenerPort;
};