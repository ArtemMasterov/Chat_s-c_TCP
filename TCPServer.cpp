#include "TCPServer.h"
#include <iostream>
#include <string>
#include <sstream>

const int MAX_BUFFER_SIZE = 4096;


TCPServer::TCPServer() { }


TCPServer::TCPServer(std::string ipAddress, int port)
	: listenerIPAddress(ipAddress), listenerPort(port) {
}

TCPServer::~TCPServer() {
	cleanupWinsock();
}

bool TCPServer::initWinsock() {

	WSADATA data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);

	if (wsInit != 0) {
		std::cout << "Error: can't initialize Winsock." << std::endl;
		return false;
	}

	return true;

}

SOCKET TCPServer::createSocket() {

	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listeningSocket != INVALID_SOCKET) {

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(listenerPort);
		inet_pton(AF_INET, listenerIPAddress.c_str(), &hint.sin_addr);

		int bindCheck = bind(listeningSocket, (sockaddr*)&hint, sizeof(hint));	

		if (bindCheck != SOCKET_ERROR) 
		{

			int listenCheck = listen(listeningSocket, SOMAXCONN);
			if (listenCheck == SOCKET_ERROR) {
				return -1;
			}
		}

		else {
			return -1;
		}

		return listeningSocket;

	}

}

void TCPServer::run() 
{

	char buf[MAX_BUFFER_SIZE];
	SOCKET listeningSocket = createSocket();

	while (true) 
	{

		if (listeningSocket == INVALID_SOCKET) 
		{
			break;
		}

		fd_set master;
		FD_ZERO(&master);

		FD_SET(listeningSocket, &master);

		while (true) 
		{

			fd_set copy = master;
			int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

			for (int i = 0; i < socketCount; i++) 
			{

				SOCKET sock = copy.fd_array[i];

				if (sock == listeningSocket) 
				{
					SOCKET client = accept(listeningSocket, nullptr, nullptr);
					FD_SET(client, &master);
					std::string welcomeMsg = "Welcome to MasterOff Chat.\n";
					send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
					std::cout << "New user joined the chat." << std::endl;

				}
				else 
				{
					ZeroMemory(buf, MAX_BUFFER_SIZE);
					int bytesReceived = recv(sock, buf, MAX_BUFFER_SIZE, 0); 

					if (bytesReceived <= 0) 
					{
						closesocket(sock);
						FD_CLR(sock, &master);
					}
					else 
					{
						for (int i = 0; i < master.fd_count; i++) 
						{ 
							SOCKET outSock = master.fd_array[i];

							if (outSock != listeningSocket) {

								if (outSock == sock) 
								{
									std::string msgSent = "Message delivered.";
									send(outSock, msgSent.c_str(), msgSent.size() + 1, 0); 	
								}
								else 
								{
									send(outSock, buf, bytesReceived, 0);
								}

							}
						}

						std::cout << std::string(buf, 0, bytesReceived) << std::endl;

					}

				}
			}
		}


	}

}

void TCPServer::sendMsg(int clientSocket, std::string msg) 
{
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}


void TCPServer::cleanupWinsock() 
{
	WSACleanup();
}